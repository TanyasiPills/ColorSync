import React, { useState, useEffect } from 'react'
import ReactDOM from 'react-dom'
import { BrowserRouter as Router, Routes, Route, useLocation } from 'react-router-dom'
import './css/Index.css'
import About from './pages/About'
import WebGLCanvas from './pages/webgl/WebGLCanvas'
import App from './App'
import SideNavbar from './components/SideNavbar_components'
import { SocialMedia } from './pages/SocialMedia'
import { Profile } from './pages/Profile'
import RightSidebar from './components/RightSidebar_components'
import { Test } from './pages/Test'
import { Search } from './pages/Search'

const MainLayout: React.FC = () => {
  const location = useLocation();
  const [isSidebarOpen, setIsSidebarOpen] = useState(true);
  const [isMobile, setIsMobile] = useState(window.innerWidth < 768);
  const [closable, setClosable] = useState(false);

  useEffect(() => {
    if (location.pathname.toLowerCase() === '/draw') {
      setClosable(true);
    } else {
      setClosable(false);
    }
  }, [location]);

  useEffect(() => {
    const handleResize = () => {
      setIsMobile(window.innerWidth < 768);
    };
    window.addEventListener('resize', handleResize);
    return () => window.removeEventListener('resize', handleResize);
  }, []);

  const toggleLeftNavbar = () => {
    if (isMobile || closable) {
      setIsSidebarOpen(!isSidebarOpen);
    }
  };

  return (
    <div className="overall-layout">
      <SideNavbar
        isOpen={(isMobile || closable) ? isSidebarOpen : true}
        onClose={toggleLeftNavbar}
        closable={isMobile || closable}
      />
      <div className="main-content">
        <Routes>
          <Route path="/" element={<SocialMedia />} />
          <Route path="/Draw" element={<WebGLCanvas />} />
          <Route path="/Download" element={<App />} />
          <Route path="/About" element={<About />} />
          <Route path="/Search" element={<Search />} />
          <Route path="/Profile" element={<Profile own />} />
          <Route path="/Profile/:id" element={<Profile />} />
          <Route path="/Test" element={<Test />} />
        </Routes>
      </div>      
      {(!isMobile && !closable) && <RightSidebar />}
    </div>
  );
}

const MainApp: React.FC = () => (
  <Router>
    <MainLayout />
  </Router>
)

ReactDOM.render(<MainApp />, document.getElementById('root'))
