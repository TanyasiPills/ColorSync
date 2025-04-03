import { useEffect, useState } from "react";
import { Card, Col, Container, Row } from "react-bootstrap";
import { backendIp } from "../constants";
import Cookies from "universal-cookie";
import { comment, post } from "../types"

export function Test() {

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

    

    return (
        <Container className="scroll-container">
            <Row>
                {post.map((post) => (
                    <Col key={post.id} md={4} sm={6} xs={12} className="mb-4">
                        <Card className="data-card">
                            <Card.Img variant="top" src={backendIp + "/users/" + post.user.id + "/pfp"} alt={post.user.username} className="card-img" />
                            <Card.Body>
                                <Card.Title>{post.text}</Card.Title>
                            </Card.Body>
                        </Card>
                    </Col>
                ))}
            </Row>
        </Container>
    );
};