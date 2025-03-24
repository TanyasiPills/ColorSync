import { useEffect, useState } from "react";
import "../css/SocialMedia.css";
import { Container, Row, Col, Card, Spinner } from 'react-bootstrap';
import { backendIp } from "../constants";
import { post, user } from "../types"
import Cookies from "universal-cookie";

export function Search() {
  const [users, setUsers] = useState<user[]>([]);
  const [posts, setPosts] = useState<post[]>([]);
  const [postsTexts, setPostsTexts] = useState<post[]>([]);
  const [postsTags, setPostsTags] = useState<post[]>([]);
  const [searchQuery, setSearchQuery] = useState<string | undefined>(undefined);
  const [show, setShow] = useState(false);
  const cookie = new Cookies();

  async function loadUsers() {
    const params: any = {};
    if (searchQuery) params.name = searchQuery;
    const result = await fetch(backendIp + '/users/search?' + new URLSearchParams(params), { method: "GET", headers: { "Accept": "application/json" } });
    if (result.ok) { setUsers(await result.json()); } else { console.log(await result.text()) }
  }

  async function loadPostsText() {
    const params: any = {};
    if (searchQuery) params.q = searchQuery;
    const result = await fetch(backendIp + '/posts/search?' + new URLSearchParams(params), { method: "GET", headers: { "Accept": "application/json" } });
    if (result.ok) { let json = await result.json(); setPostsTexts(json.data); setPosts(json.data); } else { console.log(await result.text()) }
  }

  async function loadPostsTags() {
    const params: any = {};
    if (searchQuery) params.tags = searchQuery;
    const result = await fetch(backendIp + '/posts/search?' + new URLSearchParams(params), { method: "GET", headers: { "Accept": "application/json" } });
    if (result.ok) { let json = await result.json(); setPostsTags(json.data); } else { console.log(await result.text()) }
  }

  useEffect(() => {

    loadUsers();
    loadPostsText();
    loadPostsTags();

    if (postsTexts.length < postsTags.length) {
      setPosts(postsTexts);
    }
    if (postsTags.length < postsTexts.length) {
      setPosts(postsTags);
    }
    else {
      setPosts(postsTexts);
    }

    console.log(posts);

    const homeButton = document.getElementById("homeButton")!;
    const searchButton = document.getElementById("searchButton")!;

    homeButton.addEventListener("click", () => {
      window.location.href = '/CMS';
    });
    searchButton.addEventListener("click", () => {
      window.location.href = '/CMS/SRC';
    });
  }, [searchQuery]);

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
        </Col>
        <Col id="middle" className="h-100 d-flex justify-content-center align-items-start py-4">
          <div id="feed">
            <input type="text" id="search" className="form-control mb-2 text-center" placeholder="Search..." onChange={Search} />
              <div id="userSection" className="w-100">                
                <div id="allUsers" className="overflow-auto w-100">
                  {users && users.map((user) => (
                    <div className="d-flex align-items-center p-2 border-bottom justify-content-left" data-id={user.id} onClick={takeToProfile} key={user.id}>
                      <img src={`${backendIp}/users/${user.id}/pfp`} className="rounded-circle me-2" alt="Profile" />
                      <h5 className="mb-0">{user.username}</h5>
                    </div>
                  ))}
                </div>
              </div>
              <div id="postsSection" className="d-flex flex-column w-100 flex-grow-1 overflow-auto">
                <Row id="drawings" className="w-100 justify-content-center">
                  {posts && posts.map((post) => (
                    <Col id="col" key={post.id} xs={12} sm={6} md={4} lg={4} className="d-flex justify-content-center">                    
                      <img  src={backendIp +"/images/" + post.imageId}  className="drawing-img" alt="Post"/>                    
                  </Col>
                  ))}
                </Row>
              </div>
            </div>          
        </Col>

        <Col xs="2" id="right" className="h-100"></Col>
      </Row>
    </Container>
  );
}