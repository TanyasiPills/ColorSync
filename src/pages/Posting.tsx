import { Form, Button, Alert, Spinner, Modal, ModalBody, ModalHeader, Row, Col, ListGroup } from 'react-bootstrap';
import { X } from "react-bootstrap-icons";
import { modalProp } from "../types";
import { useState } from 'react';
import "../css/Modal.css";
import Cookies from 'universal-cookie';
import { ExistingImage } from './ExistingImage';
import { backendIp } from '../constants';


export const Posting: React.FC<modalProp> = ({ show, onHide }) => {

  const [validated, setValidated] = useState(false);
  const [error, setError] = useState('');
  const [isSubmitting, setIsSubmitting] = useState(false);
  const [selectedImage, setSelectedImage] = useState<number | null>(null);
  const [showExistingImageModal, setShowExistingImageModal] = useState(false);
  const [tags, setTags] = useState<string[]>([]);
  const [text, setText] = useState("");

  function handleKeyDown(event: React.KeyboardEvent<HTMLInputElement>) {
    if (event.key === "Enter" && text.trim() !== "") {
      event.preventDefault();
      setTags([...tags, text]);
      setText("");
      
    }
  }

  function deleteTag(index: number) {
    setTags(tags.filter((_, i) => i !== index));
  }

 const handleSubmit = async (event: React.FormEvent<HTMLFormElement>) => {
     event.preventDefault();
     setError('');
     setIsSubmitting(true);
     const data: FormData = new FormData(event.target as HTMLFormElement);
 
     try {
       const res = await fetch(backendIp + '/posts', {
         method: 'POST',
         headers: {
           'Content-Type': 'application/json',
         },
         body: JSON.stringify({
           selectedImage? (imageId: selectedImage) : file: data.get("formNewImage")
           password: data.get("password"),
           email: data.get("email")
         })
       });
 
       if (!res.ok) {
         const errorData = await res.json();
         setError(errorData.message || 'Failed to Sign Up');
         setIsSubmitting(false);
         return;
       }
 
       onHide();
       window.location.href = '/'
       setError('');
       alert('Registration successful!');
     } catch (err: any) {
       setError('An error occurred: ' + err.message);
       setIsSubmitting(false);
     }
   };

  return (
    <>
      <Modal show={show} onHide={onHide} centered>
        <ModalHeader closeButton>
          <h2>Make a post</h2>
        </ModalHeader>
        <ModalBody>
          <Form noValidate validated={validated} onSubmit={handleSubmit}>
            <Row>
              <Col>
                <Form.Group controlId="formNewImage">
                  <Form.Label>New Image</Form.Label>
                  <Form.Control
                    type="file"
                    accept="image/png, image/jpeg"
                    name="newImage"
                    id="newImage"
                    className='costum-input'
                  />
                </Form.Group>
              </Col>
              <Col>
                <Form.Group controlId="formExistingImage">
                  <Form.Label>Choose Existing Image</Form.Label>
                  <Button className="mt-2" onClick={() => setShowExistingImageModal(true)}>
                    Select Image
                  </Button>
                </Form.Group>
              </Col>
            </Row>

            <Form.Group controlId="text">
              <Form.Label>Share your thoughts: </Form.Label>
              <Form.Control
                type="text"
                placeholder="Enter text"
                name='text'
                id='text'
                required
              />
            </Form.Group>

            <Form.Group controlId="tags">
              <Form.Label>Tags</Form.Label>
              <Form.Control
                type="text"
                placeholder="text"
                name='text'
                onChange={(e) => setText(e.target.value)}
                onKeyDown={handleKeyDown}
              />
              <ListGroup className="mt-3" horizontal id='tagsList'>
                {tags.map((tag, index) => (
                  <ListGroup.Item key={index} className='tagsList' onClick={() => deleteTag(index)}>
                    #{tag}
                    <X
                      size={18}
                      className="text-danger"
                      style={{ cursor: "pointer" }}
                      onClick={() => deleteTag(index)}
                    />
                  </ListGroup.Item>
                ))}
              </ListGroup>
            </Form.Group>

            <Button variant="dark" type="submit" className="w-100 mt-3" disabled={isSubmitting}>
              {isSubmitting ? <Spinner animation="border" size="sm" /> : 'Submit'}
            </Button>
          </Form>

          {error && <Alert variant="danger" className="mt-3">{error}</Alert>}
        </ModalBody>
      </Modal>
      <ExistingImage
        show={showExistingImageModal}
        onHide={() => setShowExistingImageModal(false)}
        onSelectImage={(imageId) => {
          setSelectedImage(imageId);
          setShowExistingImageModal(false);
        }}
      />
    </>
  );
}
