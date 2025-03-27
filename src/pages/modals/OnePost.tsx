import { Card, Col, Row } from "react-bootstrap";
import { backendIp } from "../../constants";
import { useEffect, useState } from "react";
import { post } from "../../types";
import { useParams } from "react-router-dom";
import "../../css/Modal.css";

export default function OnePost() {
    const { postId } = useParams();
    const [singlePost, setSinglePost] = useState<post | null>(null);

    useEffect(() => {
        async function fetchPost() {
            if (!postId) return;
            const response = await fetch(`${backendIp}/posts/${postId}`, { method: "GET" });
            if (response.ok) {
                setSinglePost(await response.json());
            }
        }
        fetchPost();
    }, [postId]);

    function takeToProfile(event: React.MouseEvent<HTMLElement>) {
        const userId = event.currentTarget.getAttribute("data-id");
        if (userId) window.location.href = `/Profile/${userId}`;
    }

    function generateDatabaseDateTime(date: Date | string) {
        const formattedDate = new Date(date);
        return new Intl.DateTimeFormat('en', {
            year: 'numeric',
            month: '2-digit',
            day: '2-digit',
            hour: '2-digit',
            minute: '2-digit',
            hour12: false,
            timeZone: 'UTC',
        }).format(formattedDate).replace(/\//g, '-').replace(',', '');
    }

    if (!singlePost) return <p>Loading post...</p>;

    return (
        <Card className="mb-3 post-card">
            <Card.Body>
                <Row className="align-items-center">
                    <Col xs="auto" className="text-center">
                        <img
                            src={`${backendIp}/users/${singlePost.user.id}/pfp`}
                            alt="Profile"
                            className="profile-img"
                            data-id={singlePost.user.id}
                            onClick={takeToProfile}
                        />
                    </Col>
                    <Col>
                        <h5 className="profile-name" data-id={singlePost.user.id} onClick={takeToProfile}>
                            {singlePost.user.username}
                        </h5>
                    </Col>
                </Row>
            </Card.Body>
            {singlePost.imageId && <Card.Img variant="top" src={`${backendIp}/images/${singlePost.imageId}`} alt="Post Image" />}
            <div className="tagsContainer">
                {singlePost.tags.map((tag, index) => (
                    <div className="tags" key={index}>#{tag}</div>
                ))}
            </div>
            <Card.Body>
                <Card.Text>{singlePost.text}</Card.Text>
            </Card.Body>
            {singlePost.comments.length > 0 && (
                <details className="mt-2">
                    <summary>Comments</summary>
                    {singlePost.comments.map((comment) => (
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
                                    <h6 data-id={comment.user.id} onClick={takeToProfile}>
                                        {comment.user.username}
                                    </h6>
                                    <p className="mb-1">{comment.text}</p>
                                    <p className="small">{generateDatabaseDateTime(comment.date)}</p>
                                </Col>
                            </Row>
                        </div>
                    ))}
                </details>
            )}
        </Card>
    );
}
