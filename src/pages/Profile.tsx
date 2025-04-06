import { useEffect, useState } from "react";
import { backendIp } from "../constants";
import "../css/Profile.css";
import { Container, Row, Col, Spinner, Button } from 'react-bootstrap';
import Cookies from "universal-cookie";
import { useParams } from "react-router-dom";
import { image, user } from "../types"
import { ProfileEdit } from "./modals/ProfileEdit";
import { PostingToProfile } from "./modals/PostingToProfile";
import { EnlargedImage } from "./modals/EnlargedImage";


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
  const [enlargeShow, setEnlargeShow] = useState(false);
  const [enlargeId, setEnlargeId] = useState<number | null>(null);


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
            <img src={backendIp + "/users/" + user.id + "/pfp"} alt="profile picture" id="profilePic" onClick={() => { if (own) setShow(true) }} />
            <h3 id="userName" onClick={() => { if (own) setShow(true) }}>{user.username}</h3>
          </div>
          <ProfileEdit show={show} onHide={() => setShow(false)} />
        </Col>
        <Col xs={6}>
          {own ? <Button onClick={() => setImgShow(true)}>Add new Image!</Button> : null}
          <PostingToProfile show={imgShow} onHide={() => setImgShow(false)} />
        </Col>
      </Row>
      <Row id="myImages" className="gx-0 gy-0 w-100">
        {imageURL && imageURL.map((e, index) => (
          <Col id="col" key={index} xs={4} className="myImages-wrapper">
            <div className="square-box">
              <img src={e} className="myImages-img"
                onClick={() => {
                  if (images) {
                    setEnlargeId(images[index].id);
                    setEnlargeShow(true);
                  }
                }} />
                <EnlargedImage show={enlargeShow} onHide={() => setEnlargeShow(false)} imageId={enlargeId ?? 0} />
            </div>
          </Col>
        ))}
      </Row>
    </Container>
  );
};
