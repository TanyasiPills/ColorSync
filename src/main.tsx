import { StrictMode } from 'react'
import { createRoot } from 'react-dom/client'
import './index.css'
import App from './App.tsx'
import { createBrowserRouter, RouterProvider } from 'react-router-dom';
import { NavBar } from './components/Navbar_components.tsx';
import WebGLCanvas from './pages/webgl/WebGLCanvas.tsx';
import Home from './pages/Home.tsx';

const router = createBrowserRouter([
  {path: "/", element: <Home/>},
  {path: "/Draw", element: <WebGLCanvas/>},
  {path: "/Download", element: <App />},
  {path: "/SignIn", element: <App />},
  {path: "/SignUp", element: <App />},
]);

createRoot(document.getElementById('root')!).render(
  <StrictMode>
    <NavBar />
    <RouterProvider router={router}></RouterProvider>
  </StrictMode>,
)
