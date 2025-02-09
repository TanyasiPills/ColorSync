import { Container, Nav, Navbar, NavDropdown } from 'react-bootstrap';
import './NavBar.css';

export function NavBar() {
  return (
    <Navbar className="navbar-custom" sticky="top" expand="lg">
      <Container>
        {/* Full Brand Name for larger screens */}
        <Navbar.Brand href="/" className="d-none d-sm-block">
        <img src="/path/to/logo.png" width="30" height="30" className="d-inline-block align-top" />
          ColorSync
        </Navbar.Brand>

        {/* Shortened Logo for small screens */}
        <Navbar.Brand href="/" className="d-block d-sm-none">
          <img src="/path/to/logo.png" width="30" height="30" className="d-inline-block align-top" />
        </Navbar.Brand>
        <Navbar.Toggle aria-controls="basic-navbar-nav" />
        <Navbar.Collapse id="basic-navbar-nav">
          <Nav className="me-auto">
            <Nav.Link href="/">Home</Nav.Link>
            <Nav.Link href="/Draw">Draw Online</Nav.Link>
            <Nav.Link href="/CMS">Colourful Media Synced</Nav.Link>
            <NavDropdown title="Account" id="basic-nav-dropdown">
              <NavDropdown.Item href="/SignIn">Sign In</NavDropdown.Item>
              <NavDropdown.Item href="/SignUp">Sign Up</NavDropdown.Item>
            </NavDropdown>
          </Nav>
        </Navbar.Collapse>
      </Container>
    </Navbar>
  );
}
