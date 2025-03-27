import { useEffect, useState } from "react";
import { backendIp } from "../constants";
import "../css/Profile.css";
import { Container, Row, Col, Spinner, Button } from 'react-bootstrap';
import Cookies from "universal-cookie";
import { useParams } from "react-router-dom";
import { image, user } from "../types"
import { ProfileEdit } from "./ProfileEdit";
import { PostingToProfile } from "./modals/PostingToProfile";


export function Profile({ own = false }: { own?: boolean }) {
  const cookie = new Cookies();
  const thisUser = cookie.get("AccessToken");
  const { id } = useParams();
  if (!own) {

    if (thisUser) {
      own = thisUser.id == id;
    }
  }

  const [user, setUser] = useState<user>();
  const [images, setImages] = useState<image[]>();
  const [imageURL, setImageURL] = useState<string[]>();
  const [show, setShow] = useState(false);
  const [imgShow, setImgShow] = useState(false);

  async function loadUsers() {
    const header: any = {};
    if (own) {
      header["Authorization"] = "Bearer " + thisUser.access_token;
    }
    const result = await fetch(backendIp + '/users' + (own ? '' : '/' + id), { method: "GET", headers: header });
    if (result.ok) {
      const resultJson: user = await result.json();
      setUser(resultJson);
      const imageResult = await fetch(backendIp + '/images/user/' + resultJson.id, { method: "GET", headers: header });
      if (imageResult.ok) {
        const imageResultJson: image[] = await imageResult.json()
        setImages(imageResultJson);
        const promises = imageResultJson.map((e) => fetch(backendIp + '/images/' + e.id, { method: "GET", headers: header }).then(res => {
          if (res.ok) {
            return res.blob();
          }
          else {
            return new Blob();
          }
        }).then(blob => URL.createObjectURL(blob)));
        console.log(await Promise.all(promises));
        setImageURL(await Promise.all(promises));
      }
    } else { console.log(await result.text()) }
  }

  useEffect(() => {
    loadUsers();
  }, [])
  if (!user) {
    return <Spinner animation="border" size="sm" />
  }
  return (
    <Container fluid className="d-flex flex-column" style={{ height: '100vh' }}>
      <Row id='profilData' className="w-100" style={{ flex: '1' }}>
        <Col xs={6} id="mainData">
          <div className="d-flex align-items-center">
            <img src={backendIp + "/users/" + user.id + "/pfp"} alt="profile picture" id="profilePic" onClick={() => {if(own) setShow(true)}}/>
            <h3 id="userName" onClick={() => {if(own) setShow(true)}}>{user.username}</h3>
          </div>
          <ProfileEdit show={show} onHide={() => setShow(false)} />
        </Col>
        <Col xs={6}>
          {own? <Button onClick={() => setImgShow(true)}>Add new Image!</Button> : null}
          <PostingToProfile show={imgShow} onHide={() => setImgShow(false)} />
        </Col>
      </Row>
      <Row id="drawings" className="w-100" style={{  }}>
        {imageURL && imageURL.map((e, index) => (
          <Col id="col" key={index} xs={12} sm={6} md={4} lg={4} className="d-flex justify-content">
            <img src={e} className="drawing-img" />
          </Col>
        ))}
      </Row>
    </Container>
  );
};
