import { Form, Button, Alert, Spinner, Modal, ModalBody, ModalHeader, Row, Col, ListGroup } from 'react-bootstrap';
import { X } from "react-bootstrap-icons";
import { modalProp } from "../../types";
import { useState } from 'react';
import "../../css/Modal.css";
import Cookies from 'universal-cookie';
import { ExistingImage } from './ExistingImage';
import { backendIp } from '../../constants';

/**
 *  what it does
 */
export const Posting: React.FC<modalProp> = ({ show, onHide }) => {

  const [validated, setValidated] = useState(false);
  const [error, setError] = useState('');
  const [isSubmitting, setIsSubmitting] = useState(false);
  const [selectedImage, setSelectedImage] = useState<number | undefined>(undefined);
  const [showExistingImageModal, setShowExistingImageModal] = useState(false);
  const [tags, setTags] = useState<string[]>([]);
  const [text, setText] = useState("");

  function handleKeyDown(event: React.KeyboardEvent<HTMLInputElement>) {
    if (event.key === "Enter" && text.trim() !== "") {
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
    const file = data.get("file");
    if (file && (file instanceof File) && file.size > 0) {
      sendingData.append("file", file);
    }
    if (selectedImage) {
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
        onSelectImage={(imageId) => {
          setSelectedImage(imageId);
          setShowExistingImageModal(false);
        }}
      />
    </>
  );
}
