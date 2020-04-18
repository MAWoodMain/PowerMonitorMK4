import * as React from "react";
import 'react-bulma-components/dist/react-bulma-components.min.css';

export class LogTable extends React.Component
{
    constructor(props) {
        super(props);
        this.state = {
            cardLog: [],
            debugLog: [],
            intervalIsSet: false,
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
        fetch("/api/log/card?max=10")
            .then(data =>
            {
                return data.json()
            })
            .then(res =>
            {
                this.setState({ cardLog: res.data })
            });
        fetch("/api/log/debug?max=10")
            .then(data =>
            {
                return data.json()
            })
            .then(res =>
            {
                this.setState({ debugLog: res.data })
            });
    };

    render() {
        const { debugLog, cardLog } = this.state;
        return (
            <div className="columns">
                <div className="column is-4"  align="center">
                    <article className="tile is-child notification has-background-grey-light">
                        <p className="title">Debug</p>
                    </article>
                    <table className="table">
                        <thead>
                        <tr>
                            <th><abbr title="Timestamp">time</abbr></th>
                            <th>level</th>
                            <th><abbr title="Message">msg</abbr></th>
                        </tr>
                        </thead>
                        <tbody>
                        {debugLog.reverse().map(dat => (
                            <tr key={dat.time + dat.msg}>
                                <th>{dat.time}</th>
                                <td>{dat.level}</td>
                                <td>{dat.msg}</td>
                            </tr>
                        ))}
                        </tbody>
                    </table>
                </div>
                <div className="column is-4"  align="center">
                    <article className="tile is-child notification has-background-grey-light">
                        <p className="title">Card Events</p>
                    </article>
                    <table className="table">
                        <thead>
                        <tr>
                            <th><abbr title="Timestamp">time</abbr></th>
                            <th><abbr title="Message">msg</abbr></th>
                        </tr>
                        </thead>
                        <tbody>
                        {cardLog.map(dat => (
                            <tr key={dat.time + dat.msg}>
                                <th>{dat.time}</th>
                                <td>{dat.msg}</td>
                            </tr>
                        ))}
                        </tbody>
                    </table>
                </div>
            </div>
        );
    }
}