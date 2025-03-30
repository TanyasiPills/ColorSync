import React, { useState } from 'react';
import ReactDOM from 'react-dom';
import { BrowserRouter as Router, Routes, Route } from 'react-router-dom';
import './css/Index.css';
import Home from './pages/Home';
import WebGLCanvas from './pages/webgl/WebGLCanvas';
import App from './App';
import SideNavbar from './components/SideNavbar_components';
import { SocialMedia } from './pages/SocialMedia';
import { Search } from 'react-bootstrap-icons';
import { Profile } from './pages/Profile';
import RightSidebar from './components/RightSideBar_components';


const MainLayout: React.FC = () => {
  const [isSidebarOpen, setIsSidebarOpen] = useState(true);

  const toggleSidebar = () => {
    setIsSidebarOpen(!isSidebarOpen);
  };

  return (
    <div className="overall-layout">
      <SideNavbar onToggleSidebar={toggleSidebar} isSidebarOpen={isSidebarOpen} />
      <div className={`content-area ${isSidebarOpen ? '' : 'collapsed'}`}>
        <button onClick={toggleSidebar} className="toggle-sidebar-button">
          {isSidebarOpen ? 'Close Sidebar' : 'Open Sidebar'}
        </button>
        <Routes>
          <Route path="/" element={<Home />} />
          <Route path="/Draw" element={<WebGLCanvas />} />
          <Route path="/Download" element={<App />} />
          <Route path="/CMS" element={<SocialMedia />} />
          <Route path="/CMS/SRC" element={<Search />} />
          <Route path="/Profile" element={<Profile own/>} />
          <Route path="/Profile/:id" element={<Profile />} />
        </Routes>
      </div>
      {isSidebarOpen && <RightSidebar />}
    </div>
  );
};

const MainApp: React.FC = () => (
  <Router>
    <MainLayout />
  </Router>
);

ReactDOM.render(<MainApp />, document.getElementById('root'));
