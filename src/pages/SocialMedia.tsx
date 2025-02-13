import { useEffect, useState } from "react";
import "./SocialMedia.css";
import { Container, Row, Col, Card } from 'react-bootstrap';
import { backendIp } from "../constants";
import { post } from "../types"

export function SocialMedia() {
  const [post, setPost] = useState<post[]>();
  useEffect(() => {
    async function load() {

      const result = await fetch(backendIp + '/post', { method: "GET", headers: { "Accept": "application/json" } });
      if (result.ok) { setPost(await result.json()); } else { console.log(await result.text()) }
    }
    load();
  })

  function generateDatabaseDateTime(date: Date | string) {
    const formattedDate = new Date(date);
    const p = new Intl.DateTimeFormat('en', {
      year: 'numeric',
      month: '2-digit',
      day: '2-digit',
      hour: '2-digit',
      minute: '2-digit',
      hour12: false,
      timeZone: 'UTC',
    }).formatToParts(formattedDate).reduce((acc: any, part) => {
      acc[part.type] = part.value;
      return acc;
    }, {});

    return `${p.year}-${p.month}-${p.day} ${p.hour}:${p.minute}`;
  }

  return (
    <Container fluid className="vh-100 d-flex flex-column">
      <Row className="flex-grow-1 w-100 h-100">
        <Col xs="2" id="left" className="h-100"></Col>
        <Col id="middle" className="h-100 d-flex justify-content-center align-items-center">
          <div id="feed">
            {post ? post.map((e) => (
              <Card className="posts" id={"" + e.id}>
                <Card.Body>
                  <Row className="align-items-center">
                    <Col md={2} className="text-center">
                      <img
                        src={backendIp + "/user/" + e.userId + "/pfp"}
                        alt="Profile"
                        className="profile-img"
                      />
                    </Col>
                    <Col md={10}>
                      <h5 className="profile-name">{e.user.username}</h5>
                    </Col>
                  </Row>
                </Card.Body>{e.imageId && <Card.Img variant="top" src={backendIp + '/image/public/' + e.imageId} alt="Card Visual" />}
                <Card.Body>
                  <Card.Text>
                    {e.text}
                  </Card.Text>
                </Card.Body>
                <Card.Footer className="card-footer">{generateDatabaseDateTime(e.date)}</Card.Footer>
                {e.comments.map((comment) => <li key={comment.id}>{comment.text}</li>)}
              </Card>
            )) : <p>loading..</p>}
          </div>
        </Col>
        <Col xs="2" id="right" className="h-100"></Col>
      </Row>
    </Container>
  );
}
