import { Form, Button, Alert, Spinner, Modal, ModalBody, ModalHeader, Row, Col } from 'react-bootstrap';
import { modalProp, user } from "../../types";
import { useState } from 'react';
import "../../css/Modal.css";
import Cookies from 'universal-cookie';
import { backendIp } from '../../constants';


export const ProfileEdit: React.FC<modalProp> = ({ show, onHide }) => {

    const [validated, setValidated] = useState(false);
    const [error, setError] = useState('');
    const [isSubmitting, setIsSubmitting] = useState(false);
    const [bioLength, setBioLength] = useState(0);
    const [user, setUser] = useState<user>();


    const cookie = new Cookies();
    const thisUser = cookie.get("AccessToken");

    if (!thisUser) {
        onHide();
        return
    }

    if (!user && thisUser) {
        setUser(thisUser);
    }

    const handleChange = (e: React.ChangeEvent<HTMLInputElement | HTMLTextAreaElement>) => {
        const { name, value } = e.target;
        setUser((prevUser) => {
            if (prevUser) {
                return { ...prevUser, [name]: value };
            }
            return prevUser;
        });
    };

    const handleSubmit = async (event: React.FormEvent<HTMLFormElement>) => {
        event.preventDefault();
        setError('');
        setIsSubmitting(true);
        const cookies = new Cookies();

        const data: FormData = new FormData(event.target as HTMLFormElement);
        const sendingData: any = {};
        const formDataFile: FormData = new FormData();

        if (data.get("bio")) {
            sendingData["profile_description"] = data.get("bio")!.toString();
        }        

        if (data.get("username")) {
            sendingData["username"] = data.get("username")!.toString();
        }
        if (data.get("email")) {
            sendingData["email"] = data.get("email")!.toString();
        }
        if (data.get("password")) {
            sendingData["password"] = data.get("password")!.toString()
        }

        console.log(sendingData);
        console.log(JSON.stringify(sendingData));

        const file = data.get("file");
        let profileImgFile: File | undefined;
        if (file && (file instanceof File) && file.size > 0) {
            formDataFile.append("file", file);
            profileImgFile = file;
        }
        try {
            const resPatch = await fetch(backendIp + '/users', {
                method: 'PATCH',
                headers: {
                    'content-type': 'application/json',
                    "Authorization": "Bearer " + cookies.get("AccessToken").access_token
                },
                body: JSON.stringify(sendingData)
            });

            if (profileImgFile) {
                const result = await fetch(backendIp + '/users/pfp', {
                    method: 'POST',
                    headers: {
                        "Authorization": "Bearer " + cookies.get("AccessToken").access_token
                    },
                    body: formDataFile
                });
                if (!result.ok) {
                    const errorData = await result.json();
                    setError(errorData.message || 'Failed to update');
                    setIsSubmitting(false);
                    return;
                }
            }

            if (!resPatch.ok) {
                const errorData = await resPatch.json();
                setError(errorData.message || 'Failed to update');
                setIsSubmitting(false);
                return;
            }

            onHide();
            setError('');
            window.location.reload();
        } catch (err: any) {
            setError('An error occurred: ' + err.message);
            setIsSubmitting(false);
        }
    };

    return (
        <>
            <Modal show={show} onHide={onHide} centered>
                <ModalHeader closeButton>
                    <h2>Prodile edit</h2>
                </ModalHeader>
                <ModalBody>
                    <Form noValidate validated={validated} onSubmit={handleSubmit}>
                        <Row>
                            <Col>
                                <Form.Group>
                                    <Form.Label>Profile image: </Form.Label>
                                    <Form.Control
                                        type="file"
                                        accept="image/png, image/jpeg"
                                        name="file"
                                        id="file"
                                        className='costum-input'
                                    />
                                </Form.Group>
                            </Col>
                            <Col>
                                <Form.Group>
                                    <Form.Label>Username: </Form.Label>
                                    <Form.Control
                                        type="text"
                                        placeholder='username'
                                        name='username'
                                        id='username'
                                        value={user?.username}
                                        onChange={handleChange}
                                    />
                                </Form.Group>
                            </Col>
                        </Row>

                        <Row>
                            <Col>
                                <Form.Group>
                                    <Form.Label>Email: </Form.Label>
                                    <Form.Control
                                        type="text"
                                        placeholder="email"
                                        name='email'
                                        id='email'
                                        value={user?.email}
                                        onChange={handleChange}
                                    />
                                </Form.Group>
                            </Col>
                            <Col>
                                <Form.Group>
                                    <Form.Label>Password: </Form.Label>
                                    <Form.Control
                                        type="password"
                                        placeholder="password   "
                                        name='password'
                                        id='password'
                                    />
                                </Form.Group>
                            </Col>
                        </Row>

                        <Row>
                            <Col>
                                <Form.Group>
                                    <Form.Label>Bio:</Form.Label>
                                    <Form.Control
                                        as="textarea"
                                        name="bio"
                                        className="custom-large-textarea"
                                        maxLength={191}
                                        rows={3}
                                        placeholder="Write something about yourself..."
                                        onInput={(e) => setBioLength((e.target as HTMLTextAreaElement).value.length)}                                        
                                    />
                                    <div className={`char-counter ${191 - bioLength <= 10 ? 'danger' : 191 - bioLength <= 50 ? 'warn' : ''}`}>
                                        {191 - bioLength} characters left
                                    </div>
                                </Form.Group>

                            </Col>
                        </Row>

                        <Button variant="dark" type="submit" className="w-100 mt-3" disabled={isSubmitting}>
                            {isSubmitting ? <Spinner animation="border" size="sm" /> : 'Submit'}
                        </Button>
                    </Form>

                    {error && <Alert variant="danger" className="mt-3">{error}</Alert>}
                </ModalBody>
            </Modal>
        </>
    );
}
