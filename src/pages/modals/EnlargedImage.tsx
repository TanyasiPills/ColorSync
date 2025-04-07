import { Modal } from "react-bootstrap";
import { backendIp } from "../../constants";
import { modalProp } from "../../types";
import "../../css/EnlargedModal.css";
import { useEffect, useState } from "react";
import Cookies from "universal-cookie";

interface EnlargedImageProps extends modalProp {
    imageId: number | null;
}

export const EnlargedImage: React.FC<EnlargedImageProps> = ({ show, onHide, imageId }) => {

    const cookie = new Cookies();
    const thisUser = cookie.get("AccessToken");

    const [imageURL, setImageURL] = useState<string | null>(null);

    useEffect(() => {
        if (imageId !== null) {
            const loadImage = async () => {
                const header: any = {
                    "Authorization": "Bearer " + thisUser.access_token,
                };

                try {
                    const result = await fetch(backendIp + '/images/' + imageId, {
                        method: "GET",
                        headers: header,
                    });

                    if (result.ok) {
                        const blob = await result.blob();
                        setImageURL(URL.createObjectURL(blob));
                    } else {
                        console.error('Failed to load image:', await result.text());
                    }
                } catch (error) {
                    console.error('Error loading image:', error);
                }
            };

            loadImage();
        }
    }, [imageId, thisUser.access_token]);

    return (
        <Modal show={show} onHide={onHide} size="lg" centered dialogClassName="transparent-modal">
            <Modal.Body className="d-flex justify-content-center p-0">
                {imageURL && (
                    <img src={imageURL} alt="enlarged"
                        className="enlarged-img"
                    />
                )}
            </Modal.Body>
        </Modal>
    );
}
