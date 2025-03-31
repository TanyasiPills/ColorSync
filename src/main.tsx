import React, { useState, useEffect } from 'react'
import ReactDOM from 'react-dom'
import { BrowserRouter as Router, Routes, Route, useLocation } from 'react-router-dom'
import './css/Index.css'
import Home from './pages/Home'
import WebGLCanvas from './pages/webgl/WebGLCanvas'
import App from './App'
import SideNavbar from './components/SideNavbar_components'
import { SocialMedia } from './pages/SocialMedia'
import { Search } from 'react-bootstrap-icons'
import { Profile } from './pages/Profile'
import RightSidebar from './components/RightSidebar_components'

const MainLayout: React.FC = () => {
  const location = useLocation()
  const [isSidebarOpen, setIsSidebarOpen] = useState(true)
  const [isMobile, setIsMobile] = useState(window.innerWidth < 768)
  const [closable, setClosable] = useState(false)

  useEffect(() => {
    // On the '/draw' page (case-insensitive) we want the sidebar to be closable
    if (location.pathname.toLowerCase() === '/draw') {
      setClosable(true)
    } else {
      setClosable(false)
    }
  }, [location])

  useEffect(() => {
    const handleResize = () => {
      setIsMobile(window.innerWidth < 768)
    }
    window.addEventListener('resize', handleResize)
    return () => window.removeEventListener('resize', handleResize)
  }, [])

  const toggleLeftNavbar = () => {
    // Allow toggling if on mobile or if the page is marked as closable (/Draw)
    if (isMobile || closable) {
      setIsSidebarOpen(!isSidebarOpen)
    }
  }

  return (
    <div className="overall-layout">
      <SideNavbar 
        isOpen={(isMobile || closable) ? isSidebarOpen : true} 
        onClose={toggleLeftNavbar} 
        closable={isMobile || closable} 
      />
      <div 
        className="main-content" 
        style={{ 
          marginLeft: (isMobile || closable) ? (isSidebarOpen ? '15%' : '0%') : '15%', 
          marginRight: (!isMobile && !closable) ? '15%' : '0%' 
        }}
      >
        <Routes>
          <Route path="/" element={<Home />} />
          <Route path="/Draw" element={<WebGLCanvas />} />
          <Route path="/Download" element={<App />} />
          <Route path="/CMS" element={<SocialMedia />} />
          <Route path="/CMS/SRC" element={<Search />} />
          <Route path="/Profile" element={<Profile own />} />
          <Route path="/Profile/:id" element={<Profile />} />
        </Routes>
      </div>
      {(!isMobile && !closable) && <RightSidebar />}
    </div>
  )
}

const MainApp: React.FC = () => (
  <Router>
    <MainLayout />
  </Router>
)

ReactDOM.render(<MainApp />, document.getElementById('root'))
