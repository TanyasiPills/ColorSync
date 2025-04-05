import { useEffect, useState } from "react";
import { backendIp } from "../constants";
import Cookies from "universal-cookie";
import { post } from "../types";
import "../css/Test.css";

export function Test() {
  const [posts, setPost] = useState<post[]>([]);
  const [likedPosts, setLikedPosts] = useState<number[]>([]);
  let loading: boolean = false;
  let offset: number | null = 0;

  const cookies = new Cookies();

  const fetchPosts = async () => {
    if (loading) return;
    loading = true;
    try {
      const result = await fetch(`${backendIp}/posts?offset=${offset}`, {
        method: "GET",
        headers: { Accept: "application/json" },
      });
      if (result.ok) {
        const json = await result.json();
        setPost((prev) => [...prev, ...json.data]);
        offset = json.offset;
      } else {
        console.log(await result.text());
      }
    } catch (error) {
      console.error(error);
    }
    loading = false;
  };

  const fetchLikes = async () => {
    try {
      const result = await fetch(`${backendIp}/users/likes`, {
        method: "GET",
        headers: {
          Accept: "application/json",
          Authorization:
            "Bearer " + cookies.get("AccessToken").access_token,
        },
      });
      if (result.ok) {
        const json = await result.json();
        setLikedPosts(json);
      } else {
        console.log(await result.text());
      }
    } catch (error) {
      console.error(error);
    }
  };

  useEffect(() => {
    fetchPosts();
    fetchLikes();

    const handleScroll = () => {
      const feed = document.getElementById("feed");
      if (!feed) return;
      const bottom =
        feed.scrollHeight - feed.scrollTop - feed.clientHeight <= 500;
      if (bottom && !loading && offset != null) {
        fetchPosts();
      }
    };

    let feed = document.getElementById("feed");
    if (feed) {
      feed.addEventListener("scroll", handleScroll);
    }
  }, []);

  function generateDatabaseDateTime(date: Date | string) {
    const formattedDate = new Date(date);
    const p = new Intl.DateTimeFormat("en", {
      year: "numeric",
      month: "2-digit",
      day: "2-digit",
      hour: "2-digit",
      minute: "2-digit",
      hour12: false,
    })
      .formatToParts(formattedDate)
      .reduce((acc: any, part) => {
        acc[part.type] = part.value;
        return acc;
      }, {});
    return `${p.year}-${p.month}-${p.day} ${p.hour}:${p.minute}`;
  }

  return (
    <div id="feed">
      {posts.map((post) => (
        <div className="responsive" key={post.id}>
          <div className="gallery">
            <div className="post-header">
              <img
                src={`${backendIp}/users/${post.user.id}/pfp`}
                alt="profile"
                id="primg"
              />
              <span className="username">{post.user.username}</span>
              <span className="date">
                {generateDatabaseDateTime(post.date)}
              </span>
            </div>
            {post.imageId && (
              <a
                target="_blank"
                rel="noopener noreferrer"
                href={`${backendIp}/images/${post.imageId}`}
              >
                <img
                  src={`${backendIp}/images/${post.imageId}`}
                  alt="post"
                  className="post-image"
                />
              </a>
            )}
            <div className="desc">
              {post.text}
            </div>
          </div>
        </div>
      ))}
      <div className="clearfix"></div>
    </div>
  );
}
