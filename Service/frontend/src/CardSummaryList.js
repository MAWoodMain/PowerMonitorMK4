import * as React from "react";
import { ButtonGroup, Container, Row, Col, Button } from 'reactstrap';
import 'bootstrap/dist/css/bootstrap.min.css';
import CardSummary from "./CardSummary";


export class CardSummaryList extends React.Component
{
    constructor(props) {
        super(props);
        this.state = {
            cards: [],
        };
    }

    componentDidMount() {
        this.getDataFromDb();
        if (!this.state.intervalIsSet) {
            let interval = setInterval(this.getDataFromDb, 1000);
            this.setState({ intervalIsSet: interval });
        }
    }

    componentWillUnmount() {
        if (this.state.intervalIsSet) {
            clearInterval(this.state.intervalIsSet);
            this.setState({ intervalIsSet: null });
        }
    }

    getDataFromDb = () => {
        fetch("/api/cards")
            .then(data =>
            {
                return data.json()
            })
            .then(res =>
            {
                this.setState({ cards: res.data })
                console.log(res.data)
            });
    };

    render() {
        const { cards } = this.state;
        console.log(cards)
        return (
            <Container>
                {cards ? cards.map((card, idx) => (
                    <CardSummary key={idx} card = {card?card:{}}/>
                )) : ""}
            </Container>
        );
    }
}