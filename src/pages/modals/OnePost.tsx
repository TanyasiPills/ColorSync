import { useEffect, useState } from "react";
import Cookies from "universal-cookie";
import { Modal, Button, InputGroup, FormControl, Row, Col } from "react-bootstrap";
import { Chat, Heart, HeartFill } from "react-bootstrap-icons";
import "../css/modal.css";
import { comment, modalProp, post } from "../../types";
import { backendIp } from "../../constants";

interface PostModalProps extends modalProp {
    postId: number;
}

export const OnePost: React.FC<PostModalProps> = ({ show, onHide, postId }) => {
    const [post, setPost] = useState<post | null>(null);
    const [newComment, setNewComment] = useState("");
    const [liked, setLiked] = useState(false);
    const cookies = new Cookies();
    const thisUser = cookies.get("AccessToken");

    const fetchPost = async () => {
        try {
            const result = await fetch(backendIp + "/posts/ " + postId, {
                headers: { "Accept": "application/json" }
            });
            if (result.ok) {
                const data = await result.json();
                setPost(data);
            }
        } catch (err) {
            console.error(err);
        }
    };

    const fetchLike = async () => {
        try {
            const res = await fetch(backendIp + "/users/likes", {
                headers: { "Authorization": "Bearer " + thisUser.access_token }
            });
            if (res.ok) {
                const likedList = await res.json();
                setLiked(likedList.includes(postId));
            }
        } catch (err) {
            console.error(err);
        }
    };

    const generateDatabaseDateTime = (date: string | Date) => {
        const now = new Date();
        const formattedDate = new Date(date);
        const diffMin = Math.floor((now.getTime() - formattedDate.getTime()) / 60000);
        if (diffMin < 60) return `${diffMin} minutes ago`;
        if (diffMin < 1440) return `${Math.floor(diffMin / 60)} hours ago`;
        if (diffMin < 5760) return `${Math.floor(diffMin / 1440)} days ago`;
        const format: Intl.DateTimeFormatOptions = diffMin > 525600
            ? { year: "numeric", month: "long", day: "numeric" }
            : { month: "long", day: "numeric" };
        return formattedDate.toLocaleDateString("en-US", format);
    };

    const handleLike = async () => {
        try {
            const res = await fetch(backendIp + "/posts/like/" + postId, {
                method: "POST",
                headers: {
                    "Authorization": "Bearer " + thisUser.access_token
                },
                body: JSON.stringify({ id: postId })
            });
            if (res.ok && post) {
                setLiked(!liked);
                setPost({ ...post, likes: post.likes + (liked ? -1 : 1) });
            }
        } catch (err) {
            console.error(err);
        }
    };

    const handleSubmit = async () => {
        try {
            const res = await fetch(backendIp + "/comments", {
                method: "POST",
                headers: {
                    "Content-Type": "application/json",
                    "Authorization": "Bearer " + thisUser.access_token
                },
                body: JSON.stringify({ postId, text: newComment })
            });
            if (res.ok) {
                const json = await res.json();
                const commentObj: comment = {
                    id: json.id,
                    text: newComment,
                    date: new Date(),
                    user: { id: thisUser.id, username: thisUser.username }
                };
                setPost(prev => prev ? { ...prev, comments: [...prev.comments, commentObj] } : null);
                setNewComment("");
            }
        } catch (err) {
            console.error(err);
        }
    };

    const takeToProfile = (id: number) => {
        window.location.href = `/Profile/${id}`;
    };

    useEffect(() => {
        if (show) {
            fetchPost();
            fetchLike();
        }
    }, [show, postId]);

    return (
        <Modal show={show} onHide={onHide} centered size="lg">
            <Modal.Header closeButton>
                <Modal.Title>{post?.user.username}'s Post</Modal.Title>
            </Modal.Header>
            <Modal.Body style={{ maxHeight: "70vh", overflowY: "auto" }}>
                {post && (
                    <>
                        <div className="post-header mb-2">
                            <img
                                className="profile-img"
                                src={backendIp + "/users/" + post.user.id + "/pfp"}
                                onClick={() => takeToProfile(post.user.id)}
                            />
                            <span className="ms-2 username" onClick={() => takeToProfile(post.user.id)}>
                                {post.user.username}
                            </span>
                            <span className="ms-auto">{generateDatabaseDateTime(post.date)}</span>
                        </div>

                        {post.imageId && (
                            <div className="post-image-wrapper text-center">
                                <img className="post-image" src={backendIp + "/images/ " + post.imageId} />
                            </div>
                        )}

                        <div className="desc mt-3">{post.text}</div>

                        {post.tags?.length > 0 && (
                            <div className="all-tags">
                                {post.tags.map((tag, i) => (
                                    <span key={i} className="tags">#{tag}</span>
                                ))}
                            </div>
                        )}

                        <div className="likes-and-comments-section d-flex align-items-center my-3">
                            <span className="like-text d-flex align-items-center me-3" onClick={handleLike}>
                                {liked ? <HeartFill className="like-icon" /> : <Heart className="like-icon" />}
                                <span className="ms-2">{post.likes}</span>
                            </span>
                        </div>

                        <div className="sticky-input mb-3">
                            <InputGroup>
                                <FormControl
                                    placeholder="Add a comment..."
                                    value={newComment}
                                    onChange={(e) => setNewComment(e.target.value)}
                                />
                                <Button variant="outline-secondary" onClick={handleSubmit}>Submit</Button>
                            </InputGroup>
                        </div>

                        <div className="comments-list">
                            {post.comments.length > 0 ? post.comments.map((c) => (
                                <div key={c.id} className="comment-container p-2">
                                    <Row className="align-items-center">
                                        <Col xs="auto">
                                            <img
                                                className="profile-img"
                                                src={backendIp + "/users/"  + c.user.id +"/pfp"}
                                                onClick={() => takeToProfile(c.user.id)}
                                            />
                                        </Col>
                                        <Col>
                                            <h6 onClick={() => takeToProfile(c.user.id)}>{c.user.username}</h6>
                                            <p className="mb-1">{c.text}</p>
                                            <p className="small">{generateDatabaseDateTime(c.date)}</p>
                                        </Col>
                                    </Row>
                                </div>
                            )) : (
                                <p className="fst-italic ps-3">No comments yet.</p>
                            )}
                        </div>
                    </>
                )}
            </Modal.Body>
        </Modal>
    );
};
