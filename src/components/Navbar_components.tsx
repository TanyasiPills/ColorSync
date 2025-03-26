import { Container, Nav, Navbar, NavDropdown } from 'react-bootstrap';
import '../css/Navbar.css';
import Cookies from 'universal-cookie';
import { useState } from 'react';
import { SignIn } from '../pages/modals/SignIn';
import { SignUp } from '../pages/modals/SignUp';

export function NavBar() {
  const cookies = new Cookies();
  const thisUser = cookies.get("AccessToken");
  const [showLogin, setShowLogin] = useState<boolean>(false);
  const [showRegister, setShowRegister] = useState<boolean>(false);



  function signOut() {    
    cookies.remove("AccessToken");
    window.location.reload();
  }
  return (
    <>
      <Navbar className="navbar-custom" variant='dark' sticky="top" expand="lg">
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
                {thisUser ?
                  <>
                    <NavDropdown.Item href={"/Profile"}>Profile</NavDropdown.Item>
                    <NavDropdown.Item onClick={signOut}>Sign out</NavDropdown.Item>
                  </> : <>
                    <NavDropdown.Item onClick={() => setShowLogin(true)}>Sign In</NavDropdown.Item>
                    <NavDropdown.Item onClick={() => setShowRegister(true)}>Sign Up</NavDropdown.Item>
                  </>}

              </NavDropdown>
            </Nav>
          </Navbar.Collapse>
        </Container>
      </Navbar>
      <SignIn show={showLogin} onHide={() => setShowLogin(false)} />
      <SignUp show={showRegister} onHide={() => setShowRegister(false)} />
    </>
  );
}
