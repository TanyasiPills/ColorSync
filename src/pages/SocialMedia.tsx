import { useEffect, useState } from "react";
import "./SocialMedia.css";
import { Container, Row, Col, Card, Button, Spinner } from 'react-bootstrap';
import { backendIp } from "../constants";
import { post } from "../types"

export function SocialMedia() {
  const [post, setPost] = useState<post[]>([]);
  useEffect(() => {
    async function load() {

      const result = await fetch(backendIp + '/posts', { method: "GET", headers: { "Accept": "application/json" } });
      if (result.ok) { setPost(await result.json()); } else { console.log(await result.text()) }
    }
    load();

    const homeButton = document.getElementById("homeButton")!;
    const searchButton = document.getElementById("searchButton")!;
    const postButton = document.getElementById("postButton")!;

    homeButton.addEventListener("click", () => {
      window.location.href = '/CMS';
    });
    searchButton.addEventListener("click", ()=> {
      window.location.href = '/CMS/SRC';
    });
    postButton.addEventListener("click", () => {
      window.location.href = 'CMS/Posting';
    });
  })
  
  function takeToProfile(event: any){
    let key: string = event.currentTarget.dataset.id;
    window.location.href = '/Profile/'+ key;
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

  console.log(post)
  return (
    <Container fluid className="vh-100 d-flex flex-column" aria-readonly>
      <Row className="flex-grow-1 w-100 h-100">
        <Col xs="2" id="left" className="h-100">
          <h3 className="costumButtons" id="homeButton">Home</h3>
          <h3 className="costumButtons" id="searchButton">Search</h3>
          <h3 className="costumButtons" id="postButton">Post</h3>
        </Col>
        <Col id="middle" className="h-100 d-flex justify-content-center align-items-center">
          <div id="feed">
            {post.length > 0 ? post.map((e) => (
              <Card className="posts" id={"" + e.id}>
                <Card.Body>
                  <Row className="align-items-center">
                    <Col md={2} className="text-center">
                      <img
                        src={backendIp + "/users/" + e.userId + "/pfp"}
                        alt="Profile"
                        className="profile-img"
                      />
                    </Col>
                    <Col md={10}>
                      <h5 className="profile-name">{e.user.username}</h5>
                    </Col>
                  </Row>
                </Card.Body>{e.imageId && <Card.Img variant="top" src={backendIp + '/images/public/' + e.imageId} alt="Card Visual" />}
                <Card.Body>
                  <Card.Text>
                    {e.text}
                  </Card.Text>
                </Card.Body>
                <Card.Footer className="card-footer">{generateDatabaseDateTime(e.date)}</Card.Footer>
                {e.comments.length > 0? e.comments.map((comment) =>
                <>
                  <Button className="commentsAll" variant="dark">Comments</Button>
                  <div className="d-flex align-items-center" id="users" >
                  <img src={backendIp + "/users/" + comment.userId + "/pfp"} data-id={e.userId} onClick={takeToProfile}/>
                  <h4 data-id={e.id} onClick={takeToProfile}>{comment.user.username}</h4>
                  <p>{comment.text}</p>
                  <p id="date">{generateDatabaseDateTime(comment.date)}</p>
                  </div>
                </>): <></>}
              </Card>
            )) : post.length == 0? <h2>No posts available at this time</h2> :  <Spinner animation="border" size="sm" />}
          </div>
        </Col>
        <Col xs="2" id="right" className="h-100"></Col>
      </Row>
    </Container>
  );
}
