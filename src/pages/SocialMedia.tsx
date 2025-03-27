import { useEffect, useState } from "react";
import "../css/SocialMedia.css";
import { Container, Row, Col, Card, Spinner, InputGroup, FormControl, Button } from 'react-bootstrap';
import { backendIp } from "../constants";
import { comment, post } from "../types"
import { Posting } from "./modals/Posting";
import Cookies from "universal-cookie";

export function SocialMedia() {

  const [post, setPost] = useState<post[]>([])
  const [show, setShow] = useState(false)
  const [newComment, setNewComment] = useState("")
  const [likedPosts, setLikedPosts] = useState<number[]>([])
  let loading: boolean = false;
  let offset: number | null = 0;

  const cookies = new Cookies()
  const thisUser = cookies.get("AccessToken")

  const fetchPosts = async () => {
    if (loading) return;
    loading = true;
    try {
      const result = await fetch(`${backendIp}/posts?offset=${offset}`, {
        method: "GET",
        headers: {
          "Accept": "application/json",         
        }
      })
      if (result.ok) {
        const json = await result.json()
        setPost(prev => [...prev, ...json.data])

        offset = json.offset;
      } else {
        console.log(await result.text())
      }
    } catch (error) {
      console.error(error)
    }
    loading = false
  }

  useEffect(() => {
    fetchPosts()

    const fetchLikes = async () => {
      try {
        const result = await fetch(`${backendIp}/users/likes`, {
          method: "GET",
          headers: {
            "Accept": "application/json",
            "Authorization": "Bearer " + cookies.get("AccessToken").access_token
          }
        })
        if (result.ok) {
          const json = await result.json()
          setLikedPosts(json)
        } else {
          console.log(await result.text())
        }
      } catch (error) {
        console.error(error)
      }
    }

    fetchLikes()


    const homeButton = document.getElementById("homeButton")
    const searchButton = document.getElementById("searchButton")

    const handleHome = () => { window.location.href = '/CMS' }
    const handleSearch = () => { window.location.href = '/CMS/SRC' }

    homeButton?.addEventListener("click", handleHome)
    searchButton?.addEventListener("click", handleSearch)

    const handleScroll = () => {
      const feed = document.getElementById("feed");
      if (!feed) return;
      const bottom = feed.scrollHeight - feed.scrollTop - feed.clientHeight <= 500;
      if (bottom && !loading) {
        if (offset != null) fetchPosts();
      }
    }

    let feed = document.getElementById("feed");
    if (feed) {
      feed.addEventListener("scroll", handleScroll);
    }

  }, [])

  function generateDatabaseDateTime(date: Date | string) {
    const formattedDate = new Date(date);
    const p = new Intl.DateTimeFormat('en', {
      year: 'numeric',
      month: '2-digit',
      day: '2-digit',
      hour: '2-digit',
      minute: '2-digit',
      hour12: false      
    }).formatToParts(formattedDate).reduce((acc: any, part) => {
      acc[part.type] = part.value;
      return acc;
    }, {});
    return `${p.year}-${p.month}-${p.day} ${p.hour}:${p.minute}`;
  }

  const handleSubmit = async (event: React.FormEvent<HTMLFormElement>, postId: number) => {
    event.preventDefault();
    const cookies = new Cookies();

    try {
      const res = await fetch(backendIp + '/comments', {
        method: 'POST',
        headers: {
          'content-type': 'application/json',
          "Authorization": "Bearer " + cookies.get("AccessToken").access_token
        },
        body: JSON.stringify({
          postId: postId,
          text: newComment
        })
      });

      if (!res.ok) {
        return;
      }

      const json = await res.json()

      const addComment: comment = {
        id: json.id,
        text: newComment,
        date: new Date(),
        user: { id: thisUser.id, username: thisUser.username },
      };

      setNewComment("");

      setPost((prevPosts) =>
        prevPosts.map((post) =>
          post.id === postId
            ? { ...post, comments: [...post.comments, addComment] }
            : post
        )
      );

    } catch (err: any) {
      console.log(err);
    }
  };

  const handleLike = async (target: HTMLElement, postId: number) => {
    const cookies = new Cookies();
    try {
      const res = await fetch(backendIp + '/posts/like/' + postId, {
        method: 'POST',
        headers: {
          "Authorization": "Bearer " + cookies.get("AccessToken").access_token
        },
        body: JSON.stringify({
          id: postId
        })
      });

      if (!res.ok) {
        return;
      }
      const numberSpan = target.children[0];
      try {
        if (numberSpan) {
          let number = parseInt(numberSpan.innerHTML);
          if (likedPosts.includes(postId)) number--;
          else number++;
          numberSpan.innerHTML = number.toString();
        }
      } catch { }
      setLikedPosts((prevLikedPosts) => {
        if (prevLikedPosts.includes(postId)) {
          return prevLikedPosts.filter((id) => id !== postId);
        } else {
          return [...prevLikedPosts, postId];
        }
      });
    } catch (err: any) {
      console.log(err);
    }
  };

  function takeToProfile(event: React.MouseEvent<HTMLImageElement | HTMLHeadingElement>) {
    const key: string = event.currentTarget.dataset.id || '';
    if (key) {
      window.location.href = `/Profile/${key}`;
    }
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
        <Col id="middle" className="h-100 d-flex justify-content-center align-items-start py-4">
          <div id="feed">
            <Posting show={show} onHide={() => setShow(false)} />
            {post.length > 0 ? post.map((post) => (
              <Card className="mb- post-card" key={post.id}>
                <Card.Body>
                  <Row className="align-items-center">
                    <Col xs="auto" className="text-center">
                      <img src={`${backendIp}/users/${post.user.id}/pfp`} alt="Profile" className="profile-img" data-id={post.user.id} key={post.id} onClick={takeToProfile} />
                    </Col>
                    <Col>
                      <h5 className="profile-name" key={post.id} data-id={post.user.id} onClick={takeToProfile}>{post.user.username}</h5>
                    </Col>
                    <Col xs="auto">
                      <p>{generateDatabaseDateTime(post.date)}</p>
                    </Col>
                  </Row>
                </Card.Body>
                {post.imageId && <Card.Img variant="top" src={`${backendIp}/images/${post.imageId}`} alt="Post Image" />}
                <div className="tagsConatiner">
                  {post.tags.length > 0 && post.tags.map((tag) => (
                    <div className="tags">#{tag}</div>
                  ))}
                </div>
                <Card.Body>
                  <Card.Text>{post.text}</Card.Text>
                </Card.Body>
                <div className="d-flex align-items-center">
                  <span
                    className="like-text d-flex align-items-center me-2"
                    onClick={(event: any) => handleLike(event.target, post.id)}>
                    {likedPosts && likedPosts.includes(post.id) ? "❤️" : "🤍"}
                    <span className="ms-3">{post.likes}</span>
                  </span>
                  {thisUser && (
                    <InputGroup className="mb-3 d-flex align-items-center">
                      <FormControl
                        placeholder="Add a comment..."
                        aria-label="New comment"
                        aria-describedby="comment-button"
                        value={newComment}
                        onChange={(e) => setNewComment(e.target.value)} />
                      <Button
                        variant="outline-secondary"
                        id="comment-button"
                        onClick={(event) => handleSubmit(event as any, post.id)}>
                        Submit
                      </Button>
                    </InputGroup>
                  )}
                </div>
                {post.comments.length > 0 && (
                  <details className="mt-2">
                    <summary className="summary">Comments</summary>
                    {post.comments.map((comment) => (
                      <div key={comment.id} className="comment-container p-2">
                        <Row className="align-items-center">
                          <Col xs="auto">
                            <img
                              className="profile-img"
                              src={`${backendIp}/users/${comment.user.id}/pfp`}
                              data-id={comment.user.id}
                              onClick={takeToProfile}
                            />
                          </Col>
                          <Col>
                            <h6 data-id={comment.user.id} onClick={takeToProfile}>{comment.user.username}</h6>
                            <p className="mb-1">{comment.text}</p>
                            <p className="small">{generateDatabaseDateTime(comment.date)}</p>
                          </Col>
                        </Row>
                      </div>
                    ))}
                  </details>
                )}
              </Card>
            )) : post.length == 0 ? <h1>There's no post at this time</h1> : <Spinner animation="border" size="sm" />}
          </div>
        </Col>
        <Col xs="2" id="right" className="h-100 d-none d-md-block"></Col>
      </Row>
    </Container>
  );
}
