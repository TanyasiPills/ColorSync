import { useEffect, useState } from "react";
import { Row, Col } from 'react-bootstrap';
import { backendIp } from "../constants";
import { post, user } from "../types"
import Cookies from "universal-cookie";
import "../css/SocialMedia.css"
import { OnePost } from "./modals/OnePost";

export function Search() {
  const [users, setUsers] = useState<user[]>([]);
  const [posts, setPosts] = useState<post[]>([]);
  const [searchQuery, setSearchQuery] = useState<string | undefined>(undefined);
  const [show, setShow] = useState(false);
  const [postId, setPostID] = useState<number>();
  const cookies = new Cookies();
  const thisUser = cookies.get("AccessToken");
  let offset: number | null = 0;
  let loading: boolean = false;


  async function loadUsers() {
    const params: any = {};
    if (searchQuery) params.name = searchQuery;
    const headers: any = { "Accept": "application/json" }
    if (thisUser) {
      headers["Authorization"] = "Bearer " + thisUser.access_token
    }
    const result = await fetch(backendIp + '/users/search?' + new URLSearchParams(params), { method: "GET", headers: headers });
    if (result.ok) { setUsers(await result.json()); } else { console.log(await result.text()) }
  }

  async function loadPosts() {
    if (loading) {
      return
    }
    loading = true;
    const tags: string[] = [];
    let text: string = "";
    if (searchQuery) {
      const regex: RegExp = /#\w+/g;
      const matches: RegExpMatchArray | null = searchQuery.match(regex);
      if (matches) {
        for (const match of matches) {
          tags.push(match.toString().substring(1));
        }
      }
      text = searchQuery.replace(/#\w+\s?/g, "");
      text = text.replace(/\s+/g, "").trim();
    }

    const params: URLSearchParams = new URLSearchParams();

    if (tags.length > 0) {
      for (const tag of tags) {
        params.append("tags", tag);
      }
    };
    if (text) params.append("q", text);
    if (offset) {
      params.append("offset", offset.toString());
    }

    const result = await fetch(backendIp + '/posts/search?' + params, { method: "GET", headers: { "Accept": "application/json" } });
    if (result.ok) {
      const json = await result.json();
      setPosts(prev => [...prev, ...json.data])
      offset = json.offset;
      loading = false;
      if (json.data.length == 0 && offset){
        loadPosts()
      } 
    } else { console.log(await result.text()), loading = false }
  }

  useEffect(() => {
    offset = 0;
    setPosts([]);
    loadPosts();
    loadUsers();
  }, [searchQuery]);

  useEffect(() => {
    const handleScroll = () => {
      const postsSection = document.getElementById("postsSection");
      if (!postsSection) return;
  
      const bottom = postsSection.scrollHeight - postsSection.scrollTop - postsSection.clientHeight <= 500;
      if (bottom && offset !== null) {
        loadPosts();
      }
    };
  
    const postsSection = document.getElementById("postsSection");
    if (postsSection) {
      postsSection.addEventListener("scroll", handleScroll);
      return () => postsSection.removeEventListener("scroll", handleScroll);
    }
  }, []);  

  function Search(event: React.ChangeEvent<HTMLInputElement>) {
    setSearchQuery(event.target.value);
  };

  function takeToProfile(event: any) {
    let key: string = event.currentTarget.dataset.id;
    window.location.href = '/Profile/' + key;
  }

  function handleImgPost(id: number) {
    setShow(true);
    setPostID(id);
  }

  return (
    <div id="searchDiv">
      <input type="text" id="search" className="form-control mb-2 text-center" placeholder="Search..." onChange={Search} />
      <div id="userSection">
        <div className="allUsers">
          {users && users.map((user) => (
            <div data-id={user.id} onClick={takeToProfile} key={user.id}>
              <Row className="align-items-center">
                <Col xs="auto">
                  <img src={`${backendIp}/users/${user.id}/pfp`} className="profile-img" alt="Profile" />
                </Col>
                <Col>
                  <h5 className="username">{user.username}</h5>
                </Col>
              </Row>
            </div>
          ))}
        </div>
      </div>
      <div id="postsSection">
        <div id="postImages">
          {posts && posts.map((post) => (
            <img src={backendIp + "/images/" + post.imageId} className="images" alt="Post" onClick={() => { handleImgPost(post.id) }} />
          ))}
          </div>
      </div>
      <OnePost show={show} onHide={() => setShow(false)} postId={postId ?? 0} />
    </div>
  );
}