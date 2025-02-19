import { useEffect, useState } from "react";
import "./SocialMedia.css";
import { Container, Row, Col, Card, Spinner } from 'react-bootstrap';
import { backendIp } from "../constants";
import { post } from "../types"

export function SocialMedia() {
  const [post, setPost] = useState<post[]>([]);
  useEffect(() => {
    async function load() {
      const result = await fetch(backendIp + '/posts', {
        method: "GET",
        headers: { "Accept": "application/json" }
      });

      if (result.ok) {
        const json = await result.json();
        setPost(json.data);
      } else {
        console.log(await result.text());
      }
    }
    load();

    const homeButton = document.getElementById("homeButton")!;
    const searchButton = document.getElementById("searchButton")!;
    const postButton = document.getElementById("postButton")!;

    homeButton.addEventListener("click", () => {
      window.location.href = '/CMS';
    });
    searchButton.addEventListener("click", () => {
      window.location.href = '/CMS/SRC';
    });
    postButton.addEventListener("click", () => {
      window.location.href = 'CMS/Posting';
    });
  })

  function takeToProfile(event: any) {
    let key: string = event.currentTarget.dataset.id;
    window.location.href = '/Profile/' + key;
  }

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
        <Col xs="2" id="left" className="h-100 d-flex flex-column align-items-center py-4">
          <h3 className="costumButtons" id="homeButton">Home</h3>
          <h3 className="costumButtons" id="searchButton">Search</h3>
          <h3 className="costumButtons" id="postButton">Post</h3>
        </Col>
        <Col id="middle" className="h-100 d-flex justify-content-center align-items-start py-4">
          <div id="feed">
            {post.length > 0 ? post.map((e) => (
              <Card className="mb- post-card" key={e.id}>
                <Card.Body>
                  <Row className="align-items-center">
                    <Col xs="auto" className="text-center">
                      <img src={`${backendIp}/users/${e.user.id}/pfp`} alt="Profile" className="profile-img" />
                    </Col>
                    <Col>
                      <h5 className="profile-name">{e.user.username}</h5>
                    </Col>
                  </Row>
                </Card.Body>
                {e.imageId && <Card.Img variant="top" src={`${backendIp}/images/public/${e.imageId}`} alt="Post Image" />}
                <Card.Body>
                  <Card.Text>{e.text}</Card.Text>
                </Card.Body>
                {e.comments.length > 0 && (
                  <details className="mt-2">
                    <summary>Comments</summary>
                    {e.comments.map((comment) => (
                      <div key={comment.id} className="comment-container p-2">
                        <Row className="align-items-center">
                          <Col xs="auto">
                            <img
                              className="profile-img"
                              src={`${backendIp}/users/${comment.user.id}/pfp`}
                              data-id={comment.user.id}
                              onClick={takeToProfile}
                            />
                          </Col>
                          <Col>
                            <h6 data-id={comment.user.id} onClick={takeToProfile}>{comment.user.username}</h6>
                            <p className="mb-1">{comment.text}</p>
                            <p className="small">{generateDatabaseDateTime(comment.date)}</p>
                          </Col>
                        </Row>
                      </div>
                    ))}
                  </details>
                )}
              </Card>
            )) : <Spinner animation="border" size="sm" />}
          </div>
        </Col>
        <Col xs="2" id="right" className="h-100 d-none d-md-block"></Col>
      </Row>
    </Container>
  );
}
