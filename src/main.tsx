import { StrictMode } from 'react';
import { createRoot } from 'react-dom/client';
import { BrowserRouter, Routes, Route, useLocation } from 'react-router-dom';
import './css/Index.css';
import App from './App';
import WebGLCanvas from './pages/webgl/WebGLCanvas';
import Home from './pages/Home';
import { SocialMedia } from './pages/SocialMedia';
import { Profile } from './pages/Profile';
import { Search } from './pages/Search';
import { TopNavbar } from './components/TopNavbar_components';
import { SideNavbar } from './components/SideNavbar_components';

function Layout({ children }: { children: React.ReactNode }) {
  const location = useLocation();
  const isDrawingPage = location.pathname === '/Draw';
  return (
    <>
      {isDrawingPage ? <TopNavbar /> : <SideNavbar />}
      {children}
    </>
  );
}

function MainApp() {
  return (
    <BrowserRouter>
      <Layout>
        <Routes>
          <Route path="/" element={<Home />} />
          <Route path="/Draw" element={<WebGLCanvas />} />
          <Route path="/Download" element={<App />} />
          <Route path="/CMS" element={<SocialMedia />} />
          <Route path="/CMS/SRC" element={<Search />} />
          <Route path="/Profile" element={<Profile own />} />
          <Route path="/Profile/:id" element={<Profile />} />
        </Routes>
      </Layout>
    </BrowserRouter>
  );
}

createRoot(document.getElementById('root')!).render(
  <StrictMode>
    <MainApp />
  </StrictMode>
);
