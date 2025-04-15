import { Container, Row, Col, Button, Card } from 'react-bootstrap'

function App() {
    return (
        <Container id="content" className="about-page py-5">
            <img src="../../public/img/transparentTitleSided.png" alt="ColorSync" className='logoAbout' />
            <p className="lead text-secondary mb-5">a way to solve my relationship</p>

            <Row className="g-4">
                <Col md={8}>
                    <h5 className="fw-semibold mb-4">
                        Unleash Your Creativity Together with ColorSync by Hry Inc. 🎨✨
                    </h5>
                    <p>
                        Create, collaborate, and connect like never before with <strong>ColorSync</strong>, the ultimate digital art platform. Whether you're working solo or side by side with friends, <strong>ColorSync</strong> empowers your creativity across <strong>online, mobile</strong>, and <strong>desktop apps</strong>.
                    </p>

                    <ul className="text-start">
                        <li>🖌️ <strong>Powerful Tools</strong> – Sketch, paint, and design with intuitive brushes and controls</li>
                        <li>🤝 <strong>Live Collaboration</strong> – Draw together in real-time with friends or the community</li>
                        <li>☁️ <strong>Cloud Sync</strong> – Access and edit your projects from anywhere</li>
                        <li>🎯 <strong>Sleek UI</strong> – Designed for beginners and pros alike</li>
                        <li>📚 <strong>Learning Hub</strong> – Tutorials and resources to boost your skills</li>
                        <li>🌐 <strong>Friendly Community</strong> – Get feedback and connect with fellow artists</li>
                    </ul>
                </Col>

                <Col md={4}>
                    <Card className="h-100 about-card">
                        <Card.Body>
                            <Card.Title>Need More Info?</Card.Title>
                            <Button as="a" href="https://github.com/TanyasiPills/ColorSync/wiki" target="_blank" rel="noopener noreferrer" className="mb-2 w-100 buttons">See our Wiki</Button>
                        </Card.Body>
                    </Card>
                </Col>
            </Row>

            <Row className="g-4 mt-4">
                <Col md={6}>
                    <Card className="h-100 about-card">
                        <Card.Body>
                            <Card.Title>Android</Card.Title>
                            <Card.Text>
                                Enjoy this <span className="rainbow-text">colorful</span> application on Android devices.
                            </Card.Text>
                            <Button variant="primary" className="mb-2 w-100 buttons">Download</Button>
                        </Card.Body>
                    </Card>
                </Col>

                <Col md={6}>
                    <Card className="h-100 about-card">
                        <Card.Body>
                            <Card.Title>Desktop</Card.Title>
                            <Card.Text>
                                Enhance your workflow with our optimized desktop experience.
                            </Card.Text>
                            <Button variant="dark" className="mb-2 w-100 buttons">Download</Button>
                        </Card.Body>
                    </Card>
                </Col>
            </Row>
        </Container>
    )
}

export default App
