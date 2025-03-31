import React, { useState } from 'react';
import { List } from 'react-bootstrap-icons';
import Cookies from 'universal-cookie';
import '../css/SideNavbar.css';
import { LeftNavbarProps } from '../types';
import { SignInAndUp } from '../pages/modals/SignIn&Up';

const SideNavbar: React.FC<LeftNavbarProps> = ({ isOpen, onClose, closable }) => {
  const cookies = new Cookies();
  const thisUser = cookies.get("AccessToken");
  const [isSignUp, setIsSignUp] = useState<boolean>(false);
  const [show, setShow] = useState<boolean>(false);

  function signOut() {
    cookies.remove("AccessToken");
    window.location.reload();
  }

  return (
    <div className={`sidebar left ${isOpen ? 'open' : 'closed'}`}>
      {closable && isOpen && (
        <List/>
      )}
      {closable && !isOpen && (
        <List className="sidebar-handle" onClick={onClose} size={18} style={{ cursor: "pointer" }} />
      )}
      <div className="sidebar-header">
        <h2>ColorSync</h2>
      </div>
      <nav>
        <a href="/" className="sidebar-link">Home</a>
        <a href="/Draw" className="sidebar-link">Draw Online</a>
        <a href="/CMS" className="sidebar-link">Colourful Media Synced</a>
        <a href="/Profile" className="sidebar-link">Profile</a>
      </nav>
      <div className="account-section">
        <details className="account-dropdown">
          <summary className="account-title">Account</summary>
          {thisUser ? (
            <div>
              <p><a href="/Profile" className="sidebar-link">Profile</a></p>
              <p onClick={signOut} className="sidebar-link">Sign Out</p>
            </div>
          ) : (
            <div>
              <p onClick={() => { setIsSignUp(false); setShow(true); }} className="sidebar-link">Sign In</p>
              <p onClick={() => { setIsSignUp(true); setShow(true); }} className="sidebar-link">Sign Up</p>
            </div>
          )}
        </details>
      </div>
      <SignInAndUp show={show} onHide={() => setShow(false)} defaultToSignUp={isSignUp} />
    </div>
  );
};

export default SideNavbar;
