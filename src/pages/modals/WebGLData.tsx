import { Form, Button, Alert, Spinner, Modal, ModalBody, ModalHeader } from 'react-bootstrap';
import { modalProp } from "../../types";
import { useState } from 'react';
import "../../css/Modal.css";
import Cookies from 'universal-cookie';
import { backendIp } from '../../constants';

interface WebGLDataProp extends modalProp {
    onData: (height: number, width: number, roomName?: string, roomPass?: string) => void;
}


export const WebGLData: React.FC<WebGLDataProp> = ({ show, onHide, onData }) => {

    const [canvasHeight, setCanvasHeight] = useState<number>(0);
    const [canvasWidth, setCanvasWidth] = useState<number>(0);
    const [roomName, setRoomName] = useState<string | undefined>(undefined);
    const [roomPass, setRoomPass] = useState<string | undefined>(undefined);


    return (
        <>
            <Modal show={show} onHide={onHide} centered>
                <ModalHeader closeButton>
                    <h2>Canvas Size or join</h2>
                </ModalHeader>
                <ModalBody>
                    <Form>
                        <Form.Group>
                            <Form.Label>Canvas Height</Form.Label>
                            <Form.Control
                                type="number"
                                value={canvasHeight ?? ""}
                                onChange={(e) => setCanvasHeight(Number(e.target.value))}
                            />
                        </Form.Group>

                        <Form.Group>
                            <Form.Label>Canvas Width</Form.Label>
                            <Form.Control
                                type="number"
                                value={canvasWidth ?? ""}
                                onChange={(e) => setCanvasWidth(Number(e.target.value))}
                            />
                        </Form.Group>

                        <Button
                            variant="success"
                            className="w-100 mt-3"
                            onClick={() => {
                                onData(canvasHeight ?? 0, canvasWidth ?? 0, false);
                                onHide();
                            }}
                        >
                            Create canvas
                        </Button>

                        <Button
                            variant="dark"
                            className="w-100 mt-3"
                            onClick={() => {
                                setShowRoomFields(true);
                            }}
                        >
                            Join room
                        </Button>

                    </Form>
                </ModalBody>
            </Modal>
        </>
    );
}
