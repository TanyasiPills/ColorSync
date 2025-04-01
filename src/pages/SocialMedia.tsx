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

  useEffect(() => {
    fetchPosts()

    fetchLikes()

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
    <div id="feed" className="container-fluid">
  <Posting show={show} onHide={() => setShow(false)} />
  {post.length > 0 ? (
    post.map((post) => (
      <div className="card mb-4 p-3" key={post.id}>
        <div className="d-flex align-items-center mb-2">
          <img
            src={backendIp + "/users/" + post.user.id + "/pfp"}
            alt="Profile"
            className="profile-img rounded-circle"
            data-id={post.user.id}
            onClick={takeToProfile}
          />
          <h5
            className="profile-name ms-2 mb-0"
            data-id={post.user.id}
            onClick={takeToProfile}
          >
            {post.user.username}
          </h5>
          <p className="post-time ms-auto text-muted small">
            {generateDatabaseDateTime(post.date)}
          </p>
        </div>
        {post.imageId && (
          <img
            className="post-img img-fluid rounded"
            src={backendIp + "/images/" + post.imageId}
            alt="Post Image"
          />
        )}
        <div className="tags-container mt-2">
          {post.tags.length > 0 &&
            post.tags.map((tag) => (
              <div className="badge bg-secondary me-1 mb-1" key={tag}>
                #{tag}
              </div>
            ))}
        </div>
        <p className="post-text mt-2">{post.text}</p>
        <div className="post-footer d-flex justify-content-between align-items-center mt-3">
          <span
            className="like-button d-flex align-items-center"
            onClick={(event: any) => handleLike(event.target, post.id)}
          >
            {likedPosts.includes(post.id) ? "❤️" : "🤍"}
            <span className="like-count ms-2">{post.likes}</span>
          </span>
          {thisUser && (
            <form
              className="comment-form d-flex w-75 mt-2"
              onSubmit={(e) => handleSubmit(e, post.id)}
            >
              <input
                type="text"
                placeholder="Add a comment..."
                value={newComment}
                onChange={(e) => setNewComment(e.target.value)}
                className="form-control me-2"
              />
              <button type="submit" className="btn btn-primary">
                Submit
              </button>
            </form>
          )}
        </div>
        {post.comments.length > 0 && (
          <details className="comments-section mt-3">
            <summary className="comments-summary fs-5">
              Comments
            </summary>
            {post.comments.map((comment) => (
              <div key={comment.id} className="comment d-flex align-items-start mt-2">
                <img
                  className="comment-profile-img rounded-circle me-2"
                  src={backendIp + "/users/" + comment.user.id + "/pfp"}
                  alt="Profile"
                  data-id={comment.user.id}
                  onClick={takeToProfile}
                />
                <div className="comment-content flex-grow-1">
                  <h6
                    className="comment-username fs-6 mb-1"
                    data-id={comment.user.id}
                    onClick={takeToProfile}
                  >
                    {comment.user.username}
                  </h6>
                  <p className="comment-text fs-6">{comment.text}</p>
                  <p className="comment-time text-muted small">
                    {generateDatabaseDateTime(comment.date)}
                  </p>
                </div>
              </div>
            ))}
          </details>
        )}
      </div>
    ))
  ) : post.length === 0 ? (
    <h1>No posts available at this time.</h1>
  ) : (
    <div className="loading-spinner">Loading...</div>
  )}
</div>

  );
}
