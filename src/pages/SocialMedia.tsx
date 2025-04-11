import { useEffect, useState } from "react";
import { backendIp } from "../constants";
import Cookies from "universal-cookie";
import { comment, post } from "../types";
import "../css/SocialMedia.css";
import { Button, Col, FormControl, InputGroup, Row } from "react-bootstrap";
import { Chat, Heart, HeartFill } from "react-bootstrap-icons";

export function SocialMedia() {
    const [post, setPost] = useState<post[]>([])
    const [visibleCommentPosts, setVisibleCommentPosts] = useState<number[]>([])
    const [newComment, setNewComment] = useState("")
    const [likedPosts, setLikedPosts] = useState<number[]>([])
    let loading: boolean = false;
    let offset: number | null = 0;

    const cookies = new Cookies();
    const thisUser = cookies.get("AccessToken");

    const fetchPosts = async () => {
        if (loading) return;
        loading = true;
        try {
            const result = await fetch(backendIp + "/posts?offset=" + offset, {
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
        const now = new Date();
        const formattedDate = new Date(date);
        let outTime: string = "";

        const diffInMilliseconds = now.getTime() - formattedDate.getTime();
        const diffInMinutes = Math.floor(diffInMilliseconds / 60000);

        if (diffInMinutes < 60) {
            outTime = diffInMinutes + " minutes ago";
        } else if (diffInMinutes < 60 * 24) {
            const hours = Math.floor(diffInMinutes / 60);
            outTime = hours + " hours ago";
        } else if (diffInMinutes < 60 * 24 * 4) {
            const days = Math.floor(diffInMinutes / (60 * 24));
            outTime = days + " days ago";
        } else if (diffInMinutes > 60 * 24 * 365) {
            const myDate: Intl.DateTimeFormatOptions = {
                year: "numeric",
                month: "long",
                day: "numeric"
            };
            outTime = formattedDate.toLocaleDateString("en-US", myDate);
        } else {
            const myDate: Intl.DateTimeFormatOptions = {
                month: "long",
                day: "numeric"
            };
            outTime = formattedDate.toLocaleDateString("en-US", myDate);
        }

        return outTime;
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
        const accessToken = cookies.get("AccessToken")?.access_token;
        if (!accessToken) return
        try {
            const res = await fetch(backendIp + '/posts/like/' + postId, {
                method: 'POST',
                headers: {
                    "Authorization": "Bearer " + accessToken
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
            setPost((prevPosts) =>
                prevPosts.map((post) =>
                    post.id === postId
                        ? { ...post, likes: post.likes + (likedPosts.includes(postId) ? -1 : 1) }
                        : post
                )
            );
        } catch (err: any) {
            console.log(err);
        }
    };

    function takeToProfile(event: React.MouseEvent<HTMLImageElement | HTMLHeadingElement>) {
        const key: string = event.currentTarget.dataset.id || '';
        if (key) {
            window.location.href = "/Profile/" + key;
        }
    }

    return (
        <div id="feed">
            {post.length > 0 ? post.map((post) => (
                <div className="post">
                    <div className="post-header">
                        <img
                            src={backendIp + "/users/" + post.user.id + "/pfp"}
                            alt="profile"
                            className="profile-img"
                            data-id={post.user.id}
                            onClick={takeToProfile}
                        />
                        <span className="username"
                            data-id={post.user.id}
                            onClick={takeToProfile}
                        >{post.user.username}</span>
                        <span className="date">
                            {generateDatabaseDateTime(post.date)}
                        </span>
                    </div>
                    <hr className="post-header-bottom" />

                    {post.imageId && (
                        <div className="post-image-wrapper">
                            <img
                                src={backendIp + "/images/" + post.imageId}
                                alt="post"
                                className="post-image"
                            />
                        </div>
                    )}

                    <div className="desc">{post.text}</div>

                    {post.tags && <div className="all-tags">{post.tags.map((tag) => (<div className="tags">#{tag}</div>))}</div>}

                    <div className="likes-and-comments-section d-flex align-items-center">
                        <span
                            className="like-text d-flex align-items-center me-3"
                            onClick={(event: React.MouseEvent<HTMLSpanElement>) =>
                                handleLike(event.currentTarget, post.id)
                            }
                        >
                            {likedPosts && likedPosts.includes(post.id) ? <HeartFill className="like-icon" /> : <Heart className="like-icon" />}
                            <span className="ms-3">{post.likes}</span>
                        </span>

                        <span
                            className="comment-text d-flex align-items-center"
                            onClick={() => {
                                setVisibleCommentPosts((prevVisible) =>
                                    prevVisible.includes(post.id)
                                        ? prevVisible.filter(id => id !== post.id)
                                        : [...prevVisible, post.id]
                                );
                            }}
                        >
                            <Chat className="comment-icon" />
                        </span>
                    </div>

                    <div className="comment-section">
                        {visibleCommentPosts.includes(post.id) && (
                            <div className="comments-container">
                                <div className="sticky-input">
                                    {thisUser && (
                                        <InputGroup className="mb-3 d-flex align-items-center">
                                            <FormControl
                                                placeholder="Add a comment..."
                                                aria-label="New comment"
                                                aria-describedby="comment-button"
                                                value={newComment}
                                                onChange={(e) => setNewComment(e.target.value)}
                                            />
                                            <Button
                                                variant="outline-secondary"
                                                id="comment-button"
                                                onClick={(event) => handleSubmit(event as any, post.id)}
                                            >
                                                Submit
                                            </Button>
                                        </InputGroup>
                                    )}
                                </div>

                                <div className="comments-list">
                                    {post.comments.length > 0 ?
                                        post.comments.map((comment) => (
                                            <div key={comment.id} className="comment-container p-2">
                                                <Row className="align-items-center">
                                                    <Col xs="auto">
                                                        <img
                                                            className="profile-img"
                                                            src={backendIp + "/users/" + comment.user.id + "/pfp"}
                                                            data-id={comment.user.id}
                                                            onClick={takeToProfile}
                                                        />
                                                    </Col>
                                                    <Col>
                                                        <h6
                                                            data-id={comment.user.id}
                                                            key={post.id}
                                                            className="date-text"
                                                        >
                                                            {comment.user.username}
                                                        </h6>
                                                        <p className="mb-1">{comment.text}</p>
                                                        <p className="small">{generateDatabaseDateTime(comment.date)}</p>
                                                    </Col>
                                                </Row>
                                            </div>
                                        )) :
                                        (
                                            thisUser ? <p className="fst-italic ps-3">Be the first to comment.</p> : <p className="fst-italic ps-3">No comments as of this time.</p>
                                        )}
                                </div>
                            </div>
                        )}
                    </div>
                </div>
            )): <h1 className="error">No posts at this time</h1>}
        </div>
    );
}
