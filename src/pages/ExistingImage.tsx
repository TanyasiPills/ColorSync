import { Col, Modal, Row } from "react-bootstrap";
import { image, modalProp } from "../types";
import { useEffect, useState } from "react";
import Cookies from "universal-cookie";
import { backendIp } from "../constants";

export const ExistingImage: React.FC<modalProp> = ({ show, onHide }) => {

    const cookie = new Cookies();
    const thisUser = cookie.get("AccessToken");

    const [images, setImages] = useState<image[]>();

    useEffect(() => {
        async function load() {
            const result = await fetch(backendIp + '/images/user/' + thisUser.id, { method: "GET", headers: { "Authorization": "Bearer" + thisUser.access_token } });
            if (result.ok) {
                setImages(await result.json());
            }
        }
    })

    return (
        <Modal show={show} onHide={onHide} centered>
            <Row id="drawings" className="w-100" style={{ display: 'flex', flexWrap: 'wrap' }}>
                {images && images.map((e) => (
                    <Col id="col" key={e.id} xs={12} sm={6} md={4} lg={4} className="d-flex justify-content">
                        <img src={backendIp + "/images/" + e.id} key={e.id} onClick={} className="drawing-img" />
                    </Col>
                ))}
            </Row>
        </Modal>
    );
}