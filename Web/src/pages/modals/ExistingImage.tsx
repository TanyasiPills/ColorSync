import { Modal } from "react-bootstrap";
import { useEffect, useState } from "react";
import Cookies from "universal-cookie";
import { backendIp } from "../../constants";
import "../../css/Modal.css";
import { image, modalProp, visibility } from "../../types";

interface ExistingImageProps extends modalProp {
    onSelectImage: (imageId: number, visibility: visibility | undefined) => void;
}

export const ExistingImage: React.FC<ExistingImageProps> = ({ show, onHide, onSelectImage }) => {

    const cookie = new Cookies();
    const thisUser = cookie.get("AccessToken");

    const [images, setImages] = useState<image[]>();
    const [imageURL, setImageURL] = useState<string[]>();

    async function load() {
        if (!thisUser) return;
        let header: any;
        if (thisUser) {
            header = {
                "Authorization": "Bearer " + thisUser.access_token
            };
        }

        try {
            const imageResult = await fetch(backendIp + '/images/user/' + thisUser.id, { method: "GET", headers: header });

            if (imageResult.ok) {
                const imageResultJson: image[] = await imageResult.json();
                setImages(imageResultJson);

                const promises = imageResultJson.map((e) =>
                    fetch(backendIp + '/images/' + e.id, { method: "GET", headers: header })
                        .then((res) => {
                            if (res.ok) {
                                return res.blob();
                            } else {
                                return new Blob();
                            }
                        })
                        .then((blob) => URL.createObjectURL(blob))
                );

                const imageURLs = await Promise.all(promises);
                setImageURL(imageURLs);
            } else {
                console.error('Failed to load images:', await imageResult.text());
            }
        } catch (error) {
            console.error('Error loading images:', error);
        }
    }

    useEffect(() => {
        load();
    }, [])

    return (
        <Modal show={show} onHide={onHide} centered>
            <h3 className="text-center mt-3">Images</h3>
            <div id="drawings">
                {images && imageURL && images.map((img, i) => (
                    <div key={img.id} className="image-item">
                        <img
                            src={imageURL[i]}
                            onClick={() => onSelectImage(img.id, img.visibility)}
                            className="drawing-img"
                        />
                    </div>
                ))}
            </div>
        </Modal>
    );
}