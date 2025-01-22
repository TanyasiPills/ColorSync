import { StrictMode } from 'react'
import { createRoot } from 'react-dom/client'
import './index.css'
import App from './App.tsx'
import { createBrowserRouter, RouterProvider } from 'react-router-dom';
import { NavBar } from './components/Navbar_components.tsx';
import WebGLCanvas from './pages/webgl/WebGLCanvas.tsx';
import Home from './pages/Home.tsx';
import { SignIn } from './pages/SignIn.tsx';
import { SignUp } from './pages/SignUp.tsx';

const router = createBrowserRouter([
  {path: "/", element: <Home/>},
  {path: "/Draw", element: <WebGLCanvas/>},
  {path: "/Download", element: <App />},
  {path: "/SignIn", element: <SignIn />},
  {path: "/SignUp", element: <SignUp />},
]);

createRoot(document.getElementById('root')!).render(
  <StrictMode>
    <NavBar />
    <RouterProvider router={router}></RouterProvider>
  </StrictMode>,
)
