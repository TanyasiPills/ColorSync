import React, { useState } from 'react';
import { Form, Button, Alert, Spinner, Modal, ModalBody, ModalHeader } from 'react-bootstrap';
import Cookies from 'universal-cookie';
import { backendIp } from '../../constants';
import { modalProp } from '../../types';
import '../../css/Modal.css';

export const SignInAndUp: React.FC<modalProp & { defaultToSignUp: boolean }> = ({ show, onHide, defaultToSignUp }) => {
  const cookie = new Cookies();
  const [isSignUp, setIsSignUp] = useState(defaultToSignUp);
  const [error, setError] = useState('');
  const [isSubmitting, setIsSubmitting] = useState(false);

  if (cookie.get("AccessToken")) {
    onHide();
    return null;
  }

  const handleSubmit = async (event: React.FormEvent<HTMLFormElement>) => {
    event.preventDefault();
    setError('');
    setIsSubmitting(true);
    const data = new FormData(event.target as HTMLFormElement);

    try {
      let url = isSignUp ? '/users' : '/users/login';
      let body = isSignUp
        ? { username: data.get("username"), email: data.get("email"), password: data.get("password") }
        : { email: data.get("email"), password: data.get("password") };

      const res = await fetch(backendIp + url, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(body)
      });

      if (!res.ok) {
        const errorData = await res.json();
        setError(errorData.message || (isSignUp ? 'Failed to Sign Up' : 'Failed to Sign In'));
        setIsSubmitting(false);
        return;
      }

      const user = await res.json();
      cookie.set("AccessToken", user);
      onHide();
      window.location.href = '/';
    } catch (err: any) {
      setError('An error occurred: ' + err.message);
      setIsSubmitting(false);
    }
  };

  return (
    <Modal show={show} onHide={onHide} centered>
      <ModalHeader closeButton>
        <h2>{isSignUp ? 'Sign Up' : 'Sign In'}</h2>
        <p>{isSignUp ? 'Create a new account' : 'Please enter your credentials to log in'}</p>
      </ModalHeader>
      <ModalBody>
        <Form onSubmit={handleSubmit}>
          {isSignUp && (
            <Form.Group controlId="formUsername">
              <Form.Label>Username</Form.Label>
              <Form.Control type="text" placeholder="Enter username" name='username' required />
            </Form.Group>
          )}

          <Form.Group controlId="formBasicEmail">
            <Form.Label>Email address</Form.Label>
            <Form.Control type="email" placeholder="Enter email" name='email' required />
            <Form.Text>We'll never share your email with anyone else.</Form.Text>
          </Form.Group>

          <Form.Group controlId="formBasicPassword">
            <Form.Label>Password</Form.Label>
            <Form.Control type="password" placeholder="Password" name='password' required />
          </Form.Group>

          {isSignUp && (
            <Form.Group controlId="formConfirmPassword">
              <Form.Label>Confirm Password</Form.Label>
              <Form.Control type="password" placeholder="Confirm Password" name='confPassword' required />
            </Form.Group>
          )}

          <Button variant="dark" type="submit" className="w-100 mt-3" disabled={isSubmitting}>
            {isSubmitting ? <Spinner animation="border" size="sm" /> : isSignUp ? 'Sign Up' : 'Login'}
          </Button>
        </Form>

        {error && <Alert variant="danger" className="mt-3">{error}</Alert>}

        <Button variant="link" className="w-100 mt-2" onClick={() => setIsSignUp(!isSignUp)}>
          {isSignUp ? 'Already have an account? Sign In' : 'New here? Sign Up'}
        </Button>
      </ModalBody>
    </Modal>
  );
};
