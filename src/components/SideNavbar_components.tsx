import React, { useEffect, useState } from 'react';
import { List } from 'react-bootstrap-icons';
import Cookies from 'universal-cookie';
import '../css/SideNavbar.css';
import { LeftNavbarProps } from '../types';
import { SignInAndUp } from '../pages/modals/SignIn&Up';
import { useLocation } from 'react-router-dom';
import { Posting } from '../pages/modals/Posting';

const SideNavbar: React.FC<LeftNavbarProps> = ({ isOpen, onClose, closable }) => {
  const cookies = new Cookies();
  const thisUser = cookies.get("AccessToken");
  const [isSignUp, setIsSignUp] = useState<boolean>(false);
  const [show, setShow] = useState<boolean>(false);
  const [showPosting, setShowPosting] = useState<boolean>(false);
  const [canPost, setCanPost] = useState<boolean>(false);
  const location = useLocation()

  function signOut() {
    cookies.remove("AccessToken");
    window.location.reload();
  }

   useEffect(() => {
      if (location.pathname.toLocaleUpperCase() === '/' && thisUser) {
        setCanPost(true)
      } else {
        setCanPost(false)
      }
    }, [location]);

  return (
    <div className={`sidebar left ${isOpen ? 'open' : 'closed'} ${closable? 'closable' : ''}`}>      
      {closable && (
        <List className="sidebar-handle" onClick={onClose} size={18} />
      )}
      <div className="sidebar-header">
        <h2>ColorSync</h2>
      </div>
      <nav>
        <a href="/" className="sidebar-link">Socail Media</a>
        {thisUser && <a href="/Draw" className="sidebar-link">Draw Online</a>}
        {canPost? <p onClick={() => setShowPosting(true)} className="sidebar-link">Post</p> : null}
        <a href="/Search" className="sidebar-link">Search</a>
        <a href="/About" className="sidebar-link">About</a>
      </nav>
      <div className="account-section">
        <details className="account-dropdown">
          <summary className="account-title">Account</summary>
          {thisUser ? (
            <div>
              <a href="/Profile" className="sidebar-link">Profile</a>
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
      <Posting show={showPosting} onHide={() => setShowPosting(false)} />
    </div>
  );
};

export default SideNavbar;
