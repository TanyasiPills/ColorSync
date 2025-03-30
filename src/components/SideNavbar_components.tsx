import Cookies from 'universal-cookie';
import { useState } from 'react';
import { SignInAndUp } from '../pages/modals/SignIn&Up';
import { Posting } from '../pages/modals/Posting';
import { useNavigate } from 'react-router-dom';

export function SideNavbar() {
  const cookies = new Cookies();
  const thisUser = cookies.get("AccessToken");
  const [isSignUp, setIsSignUp] = useState<boolean>(false);
  const [show, setShow] = useState<boolean>(false);
  const [showPost, setShowPost] = useState<boolean>(false);
  const navigate = useNavigate();

  function signOut() {    
    cookies.remove("AccessToken");
    window.location.reload();
  }

  return (
    <div className="sidebar navbar-custom d-flex flex-column align-items-center py-4">
      <h3 className="costumButtons" id="homeButton" onClick={() => navigate('/CMS')}>Home</h3>
      <h3 className="costumButtons" id="searchButton" onClick={() => navigate('/CMS/SRC')}>Search</h3>
      {thisUser ? (
        <h3 className="costumButtons" id="postButton" tabIndex={0} onClick={() => setShowPost(true)}>Post</h3>
      ) : (
        <>
          <h3 className="costumButtons" onClick={() => { setIsSignUp(false); setShow(true); }}>Sign In</h3>
          <h3 className="costumButtons" onClick={() => { setIsSignUp(true); setShow(true); }}>Sign Up</h3>
        </>
      )}
      {thisUser && (
        <h3 className="costumButtons" onClick={signOut}>Sign out</h3>
      )}

      <SignInAndUp show={show} onHide={() => setShow(false)} defaultToSignUp={isSignUp} />
      <Posting show={showPost} onHide={() => setShowPost(false)} />
    </div>
  );
}
