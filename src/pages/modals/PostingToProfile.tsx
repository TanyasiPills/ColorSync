import { Form, Button, Alert, Spinner, Modal, ModalBody, ModalHeader } from 'react-bootstrap';
import { modalProp } from "../../types";
import { useState } from 'react';
import "../../css/Modal.css";
import Cookies from 'universal-cookie';
import { backendIp } from '../../constants';


export const PostingToProfile: React.FC<modalProp> = ({ show, onHide }) => {

    const [validated, setValidated] = useState(false);
    const [error, setError] = useState('');
    const [isSubmitting, setIsSubmitting] = useState(false);

    const handleSubmit = async (event: React.FormEvent<HTMLFormElement>) => {
        event.preventDefault();
        setError('');
        setIsSubmitting(true);
        const cookies = new Cookies();

        const data: FormData = new FormData(event.target as HTMLFormElement);
        const sendingData: FormData = new FormData();

        const file = data.get("file");
        if (file && file instanceof File && file.size > 0) {
            sendingData.append("file", file);
        } else if (file) {
            setError("Invalid file format");
            setIsSubmitting(false);
            return;
        }
        
        sendingData.append("visibility", data.has("public") ? "public" : "private");

        try {
            const res = await fetch(backendIp + '/images', {
                method: 'POST',
                headers: {
                    "Authorization": "Bearer " + cookies.get("AccessToken").access_token
                },
                body: sendingData
            });

            if (!res.ok) {
                const errorData = await res.json();
                setError(errorData.message || 'Failed to post');
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
                    <h2>Add to your portfolio</h2>
                </ModalHeader>
                <ModalBody>
                    <Form noValidate validated={validated} onSubmit={handleSubmit}>

                        <Form.Group>
                            <Form.Control
                                type="file"
                                accept="image/png, image/jpeg"
                                name="file"
                                id="file"
                                className='costum-input'
                            />
                        </Form.Group>

                        <Form.Group className="mt-3">
                            <Form.Check type="checkbox" label="Make it Public" name="visibility" id='visility' />
                        </Form.Group>

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
