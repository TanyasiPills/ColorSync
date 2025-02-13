import React, { useContext, useEffect, useState } from 'react';
import { Form, Button, Alert, Spinner, Container, Row, Col } from 'react-bootstrap';
import { redirect } from 'react-router-dom';
import Cookies from 'universal-cookie';
import { backendIp } from '../constants';

export function SignIn() {
  
  const cookie = new Cookies();
  if (cookie.get("AccessToken")) {
    window.location.href = '/';
    return
  }
  const [error, setError] = useState('');
  const [isSubmitting, setIsSubmitting] = useState(false);

  const handleSubmit = async (event: React.FormEvent<HTMLFormElement>) => {
    event.preventDefault();
    const data = new FormData(event.target as HTMLFormElement);
    console.log(data.get("email") + " + " + data.get("password"));
    const response = await fetch(backendIp + '/user/login', {
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
        window.location.href = '/';
      }, 10);
      return () => clearTimeout(timer);
    },);
  };

  return (
    <Container fluid="md" className="d-flex justify-content-center align-items-center min-vh-1000">
      <Row className="w-100">
        <Col md={6} lg={4} className="mx-auto">
          <div className="text-center mb-4">
            <h2>Sign in</h2>
            <p>Please enter your credentials to log in</p>
          </div>
          {error && <Alert variant="danger">{error}</Alert>}
          <Form onSubmit={handleSubmit}>
            <Form.Group controlId="formBasicEmail">
              <Form.Label htmlFor='email'>Email address</Form.Label>
              <Form.Control
                type="email"
                placeholder="Enter email"
                id='email'
                name='email'
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
                required
              />
            </Form.Group>

            <Button variant="dark" type="submit" className="w-100 mt-3" disabled={isSubmitting}>
              {isSubmitting ? <Spinner animation="border" size="sm" /> : 'Login'}
            </Button>
          </Form>
        </Col>
      </Row>
    </Container>
  );
}
