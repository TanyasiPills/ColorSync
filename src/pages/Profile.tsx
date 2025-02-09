import "./Profile.scss";
import { Container, Row, Col } from 'react-bootstrap';

export function Profile() {
  return (
    <Container fluid className="d-flex flex-column" style={{ height: '100vh' }}>
      <Row id='profilData' className="w-100" style={{ flex: '1' }}>
        <Col xs={6} id="mainData">
          <div className="d-flex align-items-center">
            <img src={"https://picsum.photos/200/300"} alt="profile picture" id="profilePic" />
            <h3 id="userName">valami</h3>
          </div>
        </Col>
        <Col xs={6}>
          <div>Column 2</div>
        </Col>
      </Row>
      <Row id="drawings" className="w-100" style={{ flex: '3' }}>
        <Col xs={1}></Col>
      </Row>
    </Container>
  );
};
