import { useEffect, useState } from "react";
import "../css/SocialMedia.css";
import { Container, Row, Col, Card, Spinner } from 'react-bootstrap';
import { backendIp } from "../constants";
import { user } from "../types"
import Cookies from "universal-cookie";

export function Search() {
  const [users, setUsers] = useState<user[]>();
  const [searchQuery, setSearchQuery] = useState<string | undefined>(undefined);
  const [show, setShow] = useState(false);
  const cookie = new Cookies();
  const thisUser = cookie.get("AccessToken");
  useEffect(() => {
    async function load() {
      const params: any = {};
      if (searchQuery) params.name = searchQuery;
      const result = await fetch(backendIp + '/users/search?' + new URLSearchParams(params), { method: "GET", headers: { "Accept": "application/json" } });
      if (result.ok) { setUsers(await result.json()); } else { console.log(await result.text()) }
    }
    load();


    const homeButton = document.getElementById("homeButton")!;
    const searchButton = document.getElementById("searchButton")!;

    homeButton.addEventListener("click", () => {
      window.location.href = '/CMS';
    });
    searchButton.addEventListener("click", () => {
      window.location.href = '/CMS/SRC';
    });
  }, []);

  function Search(event: React.ChangeEvent<HTMLInputElement>) {
    setSearchQuery(event.target.value);
  };

  function takeToProfile(event: any) {
    let key: string = event.currentTarget.dataset.id;
    window.location.href = '/Profile/' + key;
  }

  return (
    <Container fluid className="vh-100 d-flex flex-column">
      <Row className="flex-grow-1 w-100 h-100">
        <Col xs="2" id="left" className="h-100 d-flex flex-column align-items-center py-4">
          <h3 className="costumButtons" id="homeButton">Home</h3>
          <h3 className="costumButtons" id="searchButton">Search</h3>
          {thisUser ?
            <h3 className="costumButtons" id="postButton" tabIndex={0} onClick={() => setShow(true)} >Post</h3> :
            null}
        </Col>
        <Col id="middle" className="h-100 d-flex justify-content-center align-items-center">
          <div id="feed">
            <input type="text" id="userSearch" onChange={Search} />
            <div id="allUsers">
              {users ? users.map((e) => (
                <div className="d-flex align-items-center" id="users" data-id={e.id} onClick={takeToProfile}>
                  <img src={backendIp + "/users/" + e.id + "/pfp"} />
                  <h4>{e.username}</h4>
                </div>
              )) : <Spinner animation="border" size="sm" />}
            </div>
          </div>
        </Col>
        <Col xs="2" id="right" className="h-100"></Col>
      </Row>
    </Container>
  );
}
