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

  

  return (
    <Container fluid className="vh-100 d-flex flex-column">
      <Row className="flex-grow-1 w-100 h-100">
        <Col xs="2" id="left" className="h-100"></Col>
        <Col id="middle" className="h-100 d-flex justify-content-center align-items-center">
          <div id="feed">
            
          </div>
        </Col>
        <Col xs="2" id="right" className="h-100"></Col>
      </Row>
    </Container>
  );
}
