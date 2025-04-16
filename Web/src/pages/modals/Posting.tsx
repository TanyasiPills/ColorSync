import { Form, Button, Alert, Spinner, Modal, ModalBody, ModalHeader, Row, Col, ListGroup } from 'react-bootstrap';
import { X } from "react-bootstrap-icons";
import { modalProp, visibility } from "../../types";
import { useEffect, useState } from 'react';
import "../../css/Modal.css";
import Cookies from 'universal-cookie';
import { ExistingImage } from './ExistingImage';
import { backendIp } from '../../constants';

export const Posting: React.FC<modalProp> = ({ show, onHide }) => {

  const [validated, setValidated] = useState(false);
  const [error, setError] = useState('');
  const [isSubmitting, setIsSubmitting] = useState(false);
  const [selectedImage, setSelectedImage] = useState<number | undefined>(undefined);
  const [newImage, setNewImage] = useState<File | null>(null);
  const [showExistingImageModal, setShowExistingImageModal] = useState(false);
  const [tags, setTags] = useState<string[]>([]);
  const [text, setText] = useState("");
  const [selectedImageVisibility, setSelectedImageVisibility] = useState<visibility | undefined>();


  function handleKeyDown(event: React.KeyboardEvent<HTMLInputElement>) {
      if ((event.key === "Enter" || event.key == " ") && text.trim() !== "") {
        if (!tags.includes(text)) {
          event.preventDefault();
          setTags([...tags, text]);
          setText("");
        }
    }
  }

  function deleteTag(index: number) {
    setTags(tags.filter((_, i) => i !== index));
  }

  const handleSubmit = async (event: React.FormEvent<HTMLFormElement>) => {
    event.preventDefault();
    setError('');
    setIsSubmitting(true);
    const cookies = new Cookies();

    const data: FormData = new FormData(event.target as HTMLFormElement);
    const sendingData: FormData = new FormData();
    tags.forEach((tag, index) => {
      sendingData.append(`tags[${index}]`, tag);
    });
    sendingData.append("text", data.get("text")!);

    if (newImage) {
      sendingData.append("file", newImage);
    }

    if (selectedImage && selectedImageVisibility == visibility.private) {
      const confirmed = await confirm("You selected a private image. Post it as public?");
      if (!confirmed) {
        setSelectedImage(undefined);
        setSelectedImageVisibility(undefined);        
        return;
      } else {
        sendingData.append("imageId", selectedImage.toString());
        sendingData.append("forcePost", "true");
      }
    } else if (selectedImage && selectedImageVisibility == visibility.public){
      sendingData.append("imageId", selectedImage.toString());
    }

    try {
      const res = await fetch(backendIp + '/posts', {
        method: 'POST',
        headers: {
          "Authorization": "Bearer " + cookies.get("AccessToken").access_token
        },
        body: sendingData
      });

      if (!res.ok) {
        const errorData = await res.json();
        setError(errorData.message || 'Failed to post');
        setIsSubmitting(false);
        return;
      }

      onHide();
      setError('');
    } catch (err: any) {
      setError('An error occurred: ' + err.message);
      setIsSubmitting(false);
    }
  };

  const handleFileChange = (event: React.ChangeEvent<HTMLInputElement>) => {
    const file = event.target.files?.[0] || null;
    setNewImage(file);

    if (file) {
      setSelectedImage(undefined);
    }
  };

  const handleExistingImageSelection = (imageId: number) => {
    setSelectedImage(imageId);

    setNewImage(null);
  };

  return (
    <>
      <Modal show={show} onHide={onHide} centered>
        <ModalHeader closeButton>
          <h2>Make a post</h2>
        </ModalHeader>
        <ModalBody>
          <Form noValidate validated={validated} onSubmit={handleSubmit}>

            <Form.Group>
              <Form.Label>Share your thoughts</Form.Label>
              <Form.Control
                type="text"
                placeholder="Enter text"
                name='text'
                id='text'
                required
              />
            </Form.Group>

            <Form.Group>
              <Form.Label>Tags</Form.Label>
              <Form.Control
                type="text"
                placeholder="tags"
                name='nemKuldom'
                value={text}
                onChange={(e) => setText(e.target.value)}
                onKeyDown={handleKeyDown}
              />

              <ListGroup className="mt-3" horizontal id='tagsList'>
                {tags.map((tag, index) => (
                  <ListGroup.Item key={index} className='tagsList' onClick={() => deleteTag(index)}>
                    #{tag}
                    <X
                      size={18}
                      style={{ cursor: "pointer" }}
                      onClick={() => deleteTag(index)}
                    />
                  </ListGroup.Item>
                ))}
              </ListGroup>

              <Row>
                <Col>
                  <Form.Group>
                    <Form.Label>New Image</Form.Label>
                    <Form.Control
                      type="file"
                      accept="image/png, image/jpeg"
                      name="file"
                      id="file"
                      className='costum-input'
                      onChange={handleFileChange}
                    />
                  </Form.Group>
                </Col>
                <Col>
                  <Form.Group>
                    <Form.Label>Choose Existing Image</Form.Label>
                    <Button className="mt-2" onClick={() => setShowExistingImageModal(true)}>
                      Select Image
                    </Button>
                  </Form.Group>
                </Col>
              </Row>
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
        onSelectImage={(imageId, imageVisibility) => {
          setSelectedImage(imageId);
          setSelectedImageVisibility(imageVisibility);
          handleExistingImageSelection;
          setShowExistingImageModal(false);
        }}
      />
    </>
  );
}