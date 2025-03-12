import { Form, Button, Alert, Spinner, Modal, ModalBody, ModalHeader, Row, Col } from 'react-bootstrap';
import { modalProp } from "../types";
import { useState } from 'react';
import "../css/Modal.css";
import Cookies from 'universal-cookie';

export const Posting: React.FC<modalProp> = ({ show, onHide }) => {

  const [validated, setValidated] = useState(false);
  const [error, setError] = useState('');
  const [isSubmitting, setIsSubmitting] = useState(false);

  const handleSubmit = async (event: React.FormEvent<HTMLFormElement>) => {
    throw new Error('Function not implemented.');
  }

  return (
    <Modal show={show} onHide={onHide} centered>
      <ModalHeader closeButton>
        <h2>Make a post</h2>
      </ModalHeader>
      <ModalBody>
        <Form noValidate validated={validated} onSubmit={handleSubmit}>
          <Row>
            <Col>
              <Form.Group controlId="formNewImage">
                <Form.Label>New Image</Form.Label>
                <Form.Control
                  type="file"
                  accept="image/png, image/jpeg"
                  name="newImage"
                  id="newImage"
                />
              </Form.Group>
            </Col>
            <Col>
              <Form.Group controlId="formExistingImage">
                <Form.Label>Choose Existing Image</Form.Label>
                <Button variant="primary" className="mt-2">
                  Select Image
                </Button>
              </Form.Group>
            </Col>
          </Row>

          <Form.Group controlId="formBasicEmail">
            <Form.Label>Email address</Form.Label>
            <Form.Control
              type="email"
              placeholder="Enter email"
              name='email'
              id='email'
              required
            />
            <Form.Text>
              We'll never share your email with anyone else.
            </Form.Text>
          </Form.Group>

          <Form.Group controlId="formBasicPassword">
            <Form.Label>Password</Form.Label>
            <Form.Control
              type="password"
              placeholder="Password"
              name='password'
              required
            />
          </Form.Group>

          <Form.Group controlId="formConfirmPassword">
            <Form.Label>Confirm Password</Form.Label>
            <Form.Control
              type="password"
              placeholder="Confirm Password"
              name='confPassword'
              required
            />
            {error && <Form.Text className="text-danger">{error}</Form.Text>}
          </Form.Group>

          <Button variant="dark" type="submit" className="w-100 mt-3" disabled={isSubmitting}>
            {isSubmitting ? <Spinner animation="border" size="sm" /> : 'Submit'}
          </Button>
        </Form>

        {error && <Alert variant="danger" className="mt-3">{error}</Alert>}
      </ModalBody>
    </Modal>
  );
}
