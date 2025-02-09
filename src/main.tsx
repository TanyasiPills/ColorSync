import { StrictMode } from 'react'
import { createRoot } from 'react-dom/client'
import './App.css'
import App from './App.tsx'
import { createBrowserRouter, RouterProvider } from 'react-router-dom';
import { NavBar } from './components/Navbar_components.tsx';
import WebGLCanvas from './pages/webgl/WebGLCanvas.tsx';
import Home from './pages/Home.tsx';
import { SignIn } from './pages/SignIn.tsx';
import { SignUp } from './pages/SignUp.tsx';
import { SocialMedia } from './pages/SocialMedia.tsx';
import { Profile } from './pages/Profile.tsx';

const router = createBrowserRouter([
  {path: "/", element: <Home/>},
  {path: "/Draw", element: <WebGLCanvas/>},
  {path: "/Download", element: <App />},
  {path: "/CMS", element: <SocialMedia />},
  {path: "/SignIn", element: <SignIn />},
  {path: "/SignUp", element: <SignUp />},
  {path: "/Profile", element: <Profile />},
]);

createRoot(document.getElementById('root')!).render(
  <StrictMode>
    <NavBar />
    <RouterProvider router={router}></RouterProvider>
  </StrictMode>,
)
