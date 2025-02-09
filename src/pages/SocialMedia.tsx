import "./SocialMedia.css";
import { Container, Row, Col } from 'react-bootstrap';

export function SocialMedia() {
  return (
    <Container fluid className="vh-100 d-flex">
      <Row className="flex-grow-1 w-100">
        <Col xs="2" id="left" className="h-100">
        </Col>
        <Col id="middle" className="h-100 d-flex justify-content-center align-items-center">
          <div id="feed"></div>
        </Col>
        <Col xs="2" id="right" className="h-100">
        </Col>
      </Row>
    </Container>
  );
}
