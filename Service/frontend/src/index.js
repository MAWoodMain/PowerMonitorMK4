import React from 'react';
import ReactDOM from 'react-dom';
import Log from './Log';
import registerServiceWorker from './registerServiceWorker';
import 'bootstrap/dist/css/bootstrap.min.css';
import {
    BrowserRouter,
    Switch,
    Route
} from "react-router-dom";

import 'react-bulma-components/dist/react-bulma-components.min.css';

import { library } from '@fortawesome/fontawesome-svg-core'
import { fab  } from '@fortawesome/free-brands-svg-icons'
import { faBolt, faLightbulb } from '@fortawesome/free-solid-svg-icons'
import { FontAwesomeIcon } from '@fortawesome/react-fontawesome'
import {CardSummaryList} from "./CardSummaryList";

library.add(fab, faBolt, faLightbulb)
require('dotenv').config();


ReactDOM.render(
    <BrowserRouter>
        <nav className="navbar navbar-expand-lg navbar-dark bg-dark">
            <a className="navbar-brand">
                <FontAwesomeIcon
                    icon={'bolt'}
                    color="yellow"/>
                &nbsp;Power Monitor MK4&nbsp;
                <FontAwesomeIcon
                    icon={'bolt'}
                    color="yellow"/></a>
            <button className="navbar-toggler" type="button" data-toggle="collapse" data-target="#navbarSupportedContent" aria-controls="navbarSupportedContent" aria-expanded="false"
                    aria-label="Toggle navigation">
                <span className="navbar-toggler-icon"></span>
            </button>

            <div className="collapse navbar-collapse" id="navbarSupportedContent">
                <ul className="navbar-nav mr-auto">
                    <li className="nav-item">
                        <a className="nav-link" href="/">Home <span className="sr-only">(current)</span></a>
                    </li>
                    <li className="nav-item">
                        <a className="nav-link" href="/log">Log</a>
                    </li>
                </ul>
            </div>
        </nav>
        <div>
            &nbsp;
            <Switch>
                <Route path="/" exact>
                    <CardSummaryList/>
                </Route>
                <Route path="/log" component={Log}/>
                <Route component={Error}/>
            </Switch>
        </div>
    </BrowserRouter>
    , document.getElementById('root'));

//ReactDOM.render(<App />, document.getElementById('root'));
registerServiceWorker();
