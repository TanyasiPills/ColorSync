import { Container, Nav, Navbar, NavDropdown } from 'react-bootstrap';
import { useContext } from 'react';

export function NavBar() {
  return (<><Navbar bg='dark' sticky='top' variant="dark" expand="lg" >
    <Container>
      <Navbar.Brand href="/"></Navbar.Brand>
      <Navbar.Toggle aria-controls="basic-navbar-nav" />
      <Navbar.Collapse id="basic-navbar-nav">
        <Nav className="me-auto">
          <Nav.Link href="/">Home</Nav.Link>
          <Nav.Link href="/Draw">Draw Online</Nav.Link>
          <NavDropdown>
            <NavDropdown.Item href="/SignIn">Sign In</NavDropdown.Item>
            <NavDropdown.Item href="/SignUp">Sign Up</NavDropdown.Item>
          </NavDropdown>

        </Nav>
      </Navbar.Collapse>
    </Container>
  </Navbar></>);
}