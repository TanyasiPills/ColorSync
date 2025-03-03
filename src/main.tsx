import { StrictMode } from 'react'
import { createRoot } from 'react-dom/client'
import './App.css'
import App from './App.tsx'
import { createBrowserRouter, RouterProvider } from 'react-router-dom';
import { NavBar } from './components/Navbar_components.tsx';
import WebGLCanvas from './pages/webgl/WebGLCanvas.tsx';
import Home from './pages/Home.tsx';
import { SocialMedia } from './pages/SocialMedia.tsx';
import { Profile } from './pages/Profile.tsx';
import { Search } from './pages/Search.tsx';

const router = createBrowserRouter([
  {path: "/", element: <Home/>},
  {path: "/Draw", element: <WebGLCanvas/>},
  {path: "/Download", element: <App />},
  {path: "/CMS", element: <SocialMedia />},
  {path: "/CMS/SRC", element: <Search />},
  {path: "/Profile/:id", element: <Profile own />},
]);

createRoot(document.getElementById('root')!).render(
  <StrictMode>
    <NavBar />
    <RouterProvider router={router}></RouterProvider>
  </StrictMode>,
)
