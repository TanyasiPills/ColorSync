import React, { useEffect, useState } from 'react';
import { Form, Button, Alert, Spinner, Modal, ModalBody, ModalHeader } from 'react-bootstrap';
import Cookies from 'universal-cookie';
import { backendIp } from '../constants';
import { modalProp } from '../types';
import "../css/Modal.css";

export const SignIn: React.FC<modalProp> = ({ show, onHide }) => {

  const cookie = new Cookies();

  if (cookie.get("AccessToken")) {
    onHide();
    return
  }
  const [error, setError] = useState('');
  const [isSubmitting, setIsSubmitting] = useState(false);

  const handleSubmit = async (event: React.FormEvent<HTMLFormElement>) => {
    event.preventDefault();
    const data = new FormData(event.target as HTMLFormElement);
    const response = await fetch(backendIp + '/users/login', {
      method: "POST",
      headers: { 'content-type': 'application/json' },
      body: JSON.stringify({
        email: data.get("email"),
        password: data.get("password")
      })
    });
    if (await response.ok) {
      const user = await response.json();
      cookie.set("AccessToken", user);
    }

    setError('');
    setIsSubmitting(true);
    useEffect(() => {
      const timer = setTimeout(() => {
        onHide();
        window.location.href = '/';
      }, 10);
      return () => clearTimeout(timer);
    },);
  };

  return (  
    <Modal show={show} onHide={onHide} centered>
      <ModalHeader closeButton>
        <Modal.Title>Sign In</Modal.Title>
        <p>Please enter your credentials to log in</p>
        {error && <Alert variant="danger">{error}</Alert>}
      </ModalHeader>
      <ModalBody>
      <Form onSubmit={handleSubmit}>
        <Form.Group controlId="formBasicEmail">
          <Form.Label htmlFor='email'>Email address</Form.Label>
          <Form.Control
            type="email"
            placeholder="Enter email"
            id='email'
            name='email'
            onChange={() => setIsSubmitting(false)}
            required
          />
          <Form.Text>
            We'll never share your email with anyone else.
          </Form.Text>
        </Form.Group>

        <Form.Group controlId="formBasicPassword">
          <Form.Label htmlFor='password'>Password</Form.Label>
          <Form.Control
            type="password"
            placeholder="Password"
            id='password'
            name='password'
            onChange={() => setIsSubmitting(false)}
            required
          />
        </Form.Group>

        <Button variant="dark" type="submit" className="w-100 mt-3" disabled={isSubmitting}>
          {isSubmitting ? <Spinner animation="border" size="sm" /> : 'Login'}
        </Button>
      </Form>
      </ModalBody>
    </Modal >
  );
}
