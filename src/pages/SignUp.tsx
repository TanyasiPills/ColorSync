import React, { useState } from 'react';
import { Form, Button, Alert, Spinner, Modal, ModalBody, ModalHeader } from 'react-bootstrap';
import Cookies from 'universal-cookie';
import { backendIp } from '../constants';
import { modalProp } from '../types';

export const SignUp: React.FC<modalProp> = ({ show, onHide }) => {

  const cookie = new Cookies();
  if (cookie.get("AccessToken")) {
    onHide();
    window.location.href = '/';
    return
  }
  const [validated, setValidated] = useState(false);
  const [error, setError] = useState('');
  const [isSubmitting, setIsSubmitting] = useState(false);

  const handleSubmit = async (event: React.FormEvent<HTMLFormElement>) => {
    event.preventDefault();
    setError('');
    setIsSubmitting(true);
    const data: FormData = new FormData(event.target as HTMLFormElement);

    try {
      const res = await fetch(backendIp + '/users', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify({
          username: data.get("username"),
          password: data.get("password"),
          email: data.get("email")
        })
      });

      if (!res.ok) {
        const errorData = await res.json();
        setError(errorData.message || 'Failed to Sign Up');
        setIsSubmitting(false);
        return;
      }

      onHide();
      window.location.href = '/'
      setError('');
      alert('Registration successful!');
    } catch (err: any) {
      setError('An error occurred: ' + err.message);
      setIsSubmitting(false);
    }
  };

  return (

    <Modal show={show} onHide={onHide} centered>
      <ModalHeader closeButton>
        <h2>Sign Up</h2>
        <p>Create a new account</p>
      </ModalHeader>
      <ModalBody>
        <Form noValidate validated={validated} onSubmit={handleSubmit}>
          <Form.Group controlId="formUsername">
            <Form.Label>Username</Form.Label>
            <Form.Control
              type="text"
              placeholder="Enter username"
              name='username'
              id='username'
              required
            />
          </Form.Group>

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
};
