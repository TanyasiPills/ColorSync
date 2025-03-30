import React, { useState } from 'react';
import { Navbar, Nav, Dropdown } from 'react-bootstrap';
import { Link } from 'react-router-dom';
import '../css/SideNavbar.css';

const SideNavbar: React.FC<{ onToggleSidebar: () => void; isSidebarOpen: boolean }> = ({ onToggleSidebar, isSidebarOpen }) => {
  const [isDropdownOpen, setIsDropdownOpen] = useState(false);

  return (
    <div id="side-navbar" className={`sidebar ${isSidebarOpen ? '' : 'collapsed'}`}>
      <Navbar expand="lg" bg="dark" variant="dark" className="h-100">
        <Navbar.Brand href="#">ColorSync</Navbar.Brand>
        <Navbar.Toggle aria-controls="responsive-navbar-nav" onClick={onToggleSidebar} />
        <Navbar.Collapse id="responsive-navbar-nav">
          <Nav className="flex-column">
            <Nav.Link as={Link} to="/">Home</Nav.Link>
            <Nav.Link as={Link} to="/Draw">Draw</Nav.Link>
            <Nav.Link as={Link} to="/Download">Download</Nav.Link>
            <Nav.Link as={Link} to="/CMS">Social Media</Nav.Link>
            <Dropdown show={isDropdownOpen} onToggle={() => setIsDropdownOpen(!isDropdownOpen)}>
              <Dropdown.Toggle id="dropdown-basic">
                Dropdown
              </Dropdown.Toggle>
              <Dropdown.Menu>
                <Dropdown.Item href="#">Action</Dropdown.Item>
                <Dropdown.Item href="#">Another action</Dropdown.Item>
                <Dropdown.Item href="#">Something else</Dropdown.Item>
              </Dropdown.Menu>
            </Dropdown>
          </Nav>
        </Navbar.Collapse>
      </Navbar>
    </div>
  );
};

export default SideNavbar;
