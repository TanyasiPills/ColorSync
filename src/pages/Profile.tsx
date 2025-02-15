import { useEffect, useState } from "react";
import { backendIp } from "../constants";
import "./Profile.css";
import { Container, Row, Col, Spinner } from 'react-bootstrap';
import Cookies from "universal-cookie";
import { useParams } from "react-router-dom";
import { image, user } from "../types"


export function Profile({ own = false }: { own: boolean }) {
  //username, id, profile kép, email, 
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


  useEffect(() => {
    async function load() {
      const header: any = {  };
      if (own) {
        header["Authorization"] = "Bearer " + thisUser.access_token;
      }
      const result = await fetch(backendIp + '/users' + (own ? '' : '/' + id), { method: "GET", headers: header });
      if (result.ok) {
        const resultJson:user = await result.json();
        setUser(resultJson);
        const imageResult = await fetch(backendIp + '/image/users/' + resultJson.id, {method: "GET", headers: header});
        if (imageResult.ok) {
          const imageResultJson:image[] = await imageResult.json() 
          setImages(imageResultJson);
          const promises = imageResultJson.map((e)=> fetch(backendIp+'/images/' + e.id, {method: "GET", headers: header}).then(res => {
            if (res.ok) {
              return res.blob();
            }
            else{
              throw new Error("failed to fetch image");
            }
          }).then(blob => URL.createObjectURL(blob)));
          setImageURL(await Promise.all(promises));
        }
      } else { console.log(await result.text()) }
    }
    load();
  })
  if (!user) {
    return <Spinner animation="border" size="sm" />
  }
  return (
    <Container fluid className="d-flex flex-column" style={{ height: '100vh' }}>
      <Row id='profilData' className="w-100" style={{ flex: '1' }}>
        <Col xs={6} id="mainData">
          <div className="d-flex align-items-center">
            <img src={backendIp + "/user/" + user.id + "/pfp"} alt="profile picture" id="profilePic" />
            <h3 id="userName">{user.username}</h3>
          </div>
        </Col>
        <Col xs={6}>
        </Col>
      </Row>
      <Row id="drawings" className="w-100" style={{ flex: '3' }}>
        {imageURL&&imageURL.map((e) => (
          <Col xs={12} sm={6} md={4} lg={2}>
            <img src={e} alt="" style={{ width: '100%' }} />
          </Col>
        ))}
      </Row>
    </Container>
  );
};
