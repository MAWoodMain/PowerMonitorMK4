import * as React from "react";
import 'react-bulma-components/dist/react-bulma-components.min.css';
import axios from "axios";

export class UpdateItemForm extends React.Component
{
    constructor(props) {
        super(props);
        this.state = {
            idToUpdate: 0,
            updateToApply: "",
        };
    }

    updateDB = (idToUpdate, updateToApply) => {
        let objIdToUpdate = null;
        this.props.data.forEach(dat => {
            if (dat.id === idToUpdate) {
                objIdToUpdate = dat._id;
            }
        });

        axios.post("http://powermonitor-mk4:3001/api/data/updateData", {
            id: objIdToUpdate,
            update: { message: updateToApply }
        });
    };

    render() {
        return (
            <div className="Box">
                <input
                    className="input"
                    type="text"
                    style={{ width: "100px" }}
                    onChange={e => this.setState({ idToUpdate: e.target.value })}
                    placeholder="id"
                />
                <input
                    className="input"
                    type="text"
                    style={{ width: "200px" }}
                    onChange={e => this.setState({ updateToApply: e.target.value })}
                    placeholder="new value"
                />
                <button className="button"
                    onClick={() =>
                        this.updateDB(this.state.idToUpdate, this.state.updateToApply)
                    }
                >
                    UPDATE
                </button>
            </div>
        );
    }
}