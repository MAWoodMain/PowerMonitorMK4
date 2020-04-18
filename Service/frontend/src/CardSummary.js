import * as React from "react";
import {Table, ButtonGroup, Container, Row, Col, Button, Modal, ModalHeader, ModalBody, ModalFooter} from 'reactstrap';
import 'bootstrap/dist/css/bootstrap.min.css';
import {useState} from "react";

const CardSummary = (props) => {
    const {
        card,
    } = props;
    const [modal, setModal] = useState(false);
    const toggle = () => setModal(!modal);
    return (
        <Row>
            <Col sm={{size: 3}}>
                <Container fluid className={
                    card.state === "RUNNING"?
                        "text-center bg-success"
                        :
                        card.state === "CONFIGURED"?
                            "text-center bg-info"
                            :
                            card.state === "UNIDENTIFIED"?
                                "text-center bg-warning"
                                :
                                "text-center bg-danger"
                }>
                    <p>
                        <h1 className="display-4">{card.type?card.type:"Unknown"}</h1>
                    </p>
                    <p>
                        <ButtonGroup>
                            {card.state === "RUNNING"?<Button className="bg-danger" onClick={toggle}><a>Stop</a></Button>:""}
                            {card.state === "CONFIGURED"?<Button className="bg-success" onClick={toggle}>Start</Button>:""}
                            {card.state === "UNIDENTIFIED"?<Button className="bg-primary" onClick={toggle}>Configure</Button>:""}
                            <Button onClick={toggle}>Info</Button>
                            <Button>Settings</Button>
                        </ButtonGroup>
                    </p>
                </Container>
            </Col>

            {card.channels ? card.channels.map((channel, idx) => (
                <Col key={idx}>
                    <Container fluid className="text-center bg-dark text-white">
                        <p>
                            <h1 className="display-4">{channel.name}</h1>
                        </p>
                        <p>
                            <ButtonGroup>
                                <Button>Mapping</Button>
                                <Button>Calibration</Button>
                            </ButtonGroup>
                        </p>
                    </Container>
                </Col>
            )) : ""}
            <Modal isOpen={modal} toggle={toggle}>
                <ModalHeader toggle={toggle}>Modal title</ModalHeader>
                <ModalBody>
                    <Table responsive>
                        <tbody>
                        <tr>
                            <td>Card Type</td>
                            <td>{card.type}</td>
                        </tr>
                        </tbody>
                    </Table>
                </ModalBody>
                <ModalFooter>
                    <Button color="primary" onClick={toggle}>Close</Button>
                </ModalFooter>
            </Modal>
        </Row>
    );
}
export default CardSummary;