/*!CK:3683115083!*//*1404702378,*/

if (self.CavalryLogger) { CavalryLogger.start_js(["sVsy7"]); }

__d("XUITokenizerRichToken.react",["DraggableTokenMixin.react","ReactPropTypes","React","SearchableEntry","XUICloseButton.react","cx","tx"],function(a,b,c,d,e,f,g,h,i,j,k,l,m){var n=i.createClass({displayName:'XUITokenizerRichToken',mixins:[g],propTypes:{label:h.string.isRequired,entry:h.instanceOf(j),onRemove:h.func,highlighted:h.bool},_onRemove:function(o){o.preventDefault();this.props.onRemove(this.props.entry);},render:function(){var o;if(this.props.onRemove)o=k({type:"button",'aria-label':m._("Remove {item}",{item:this.props.label}),className:"_5yii",size:"small",onClick:this._onRemove});var p=this.props.entry;return this.renderDraggableToken(this.transferPropsTo(i.DOM.span(null,i.DOM.div({className:"_5yij",label:null},i.DOM.img({className:"_5yik",src:p.getPhoto()}),p.getTitle()," - ",p.getSubtitle(),o))));}});e.exports=n;},null);
__d("XUITokenizer.react",["AbstractTokenizer.react","ReactPropTypes","React","XUITokenizerToken.react","XUITokenizerRichToken.react","XUITypeaheadTextOnlyView.react","XUITypeaheadView.react","cx","joinClasses"],function(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o){var p={ViewRenderer:m,TokenRenderer:j,useLayer:true},q={ViewRenderer:l,TokenRenderer:j,useLayer:true},r=i.createClass({displayName:'XUITokenizer',propTypes:{className:h.string,entries:h.array,excludedEntries:h.object,onAddEntryAttempt:h.func.isRequired,onQueryStringChange:h.func,onRemoveEntryAttempt:h.func.isRequired,onReorderEntryAttempt:h.func,placeholder:h.string,queryString:h.string,searchSource:h.object.isRequired,selectOnBlur:h.bool,testID:h.string,viewStyle:h.oneOf(['textonly','rich'])},getDefaultProps:function(){return {viewStyle:'rich'};},render:function(){var s=o(this.props.className,"_59_m"),t=this.props.viewStyle=='rich'?p:q;if(this.props.tokenStyle=='rich')t.TokenRenderer=k;return (i.DOM.div({className:s,'data-testid':this.props.testID},g({autoHighlight:this.props.autoHighlight,className:"_59_n",entries:this.props.entries,excludedEntries:this.props.excludedEntries,onAddEntryAttempt:this.props.onAddEntryAttempt,onEnter:this.props.onEnter,onInputBlur:this.props.onInputBlur,onQueryStringChange:this.props.onQueryStringChange,onRemoveEntryAttempt:this.props.onRemoveEntryAttempt,onReorderEntryAttempt:this.props.onReorderEntryAttempt,onEnterWithoutSelection:this.props.onEnterWithoutSelection,placeholder:this.props.placeholder,presenter:t,queryString:this.props.queryString,ref:"tokenizer",searchSource:this.props.searchSource,selectOnBlur:this.props.selectOnBlur,showEntriesOnFocus:this.props.showEntriesOnFocus})));},focusInput:function(){this.refs.tokenizer&&this.refs.tokenizer.focusInput();}});e.exports=r;},null);
__d("XUITokenizerWrapper.react",["ReactPropTypes","React","XUITokenizer.react","createObjectFrom"],function(a,b,c,d,e,f,g,h,i,j){var k=h.createClass({displayName:'XUITokenizerWrapper',propTypes:{name:g.string.isRequired,className:g.string,placeholder:g.string,initialEntries:g.array,viewStyle:g.oneOf(['textonly','rich']).isRequired,tokenStyle:g.oneOf(['textonly','rich']),selectOnBlur:g.bool,searchSource:g.object.isRequired,excludedEntries:g.array,testID:g.string},getInitialState:function(){return {entries:this.props.initialEntries||[]};},_renderHiddenInput:function(l){var m=l.getUniqueID();return (h.DOM.input({key:m,type:"hidden",name:this.props.name+'[]',value:m}));},_onAddEntry:function(l){this.state.entries.push(l);this.forceUpdate();},_onRemoveEntry:function(l){var m=this.state.entries.indexOf(l);this.state.entries.splice(m,1);this.forceUpdate();},render:function(){return (h.DOM.div(null,this.state.entries.map(this._renderHiddenInput),i({key:"tokenizer",className:this.props.className,placeholder:this.props.placeholder,searchSource:this.props.searchSource,entries:this.state.entries,excludedEntries:j(this.props.excludedEntries,true),viewStyle:this.props.viewStyle,tokenStyle:this.props.tokenStyle,onAddEntryAttempt:this._onAddEntry,onRemoveEntryAttempt:this._onRemoveEntry,selectOnBlur:this.props.selectOnBlur,testID:this.props.testID})));}});e.exports=k;},null);
__d("DevappImageSelector",["Event","copyProperties"],function(a,b,c,d,e,f,g,h){function i(j,k,l){g.listen(j,'click',function(){i.getPreviewDialog().show(k,l);});}h(i,{_previewDialog:null,setPreviewDialog:function(j){i._previewDialog=j;},getPreviewDialog:function(){return i._previewDialog;}});e.exports=i;},null);
__d("DeveloperAppColorPicker",["Event","Style"],function(a,b,c,d,e,f,g,h){function i(j,k){if(k.trim()===''){h.set(j,'backgroundColor','transparent');return;}h.set(j,'backgroundColor','#'+k);}e.exports={bindColorPickerInputAndPreview:function(j,k){var l=j.getAttribute('value');i(k,l);g.listen(j,'input',function(event){var m=new RegExp(("^[0-9a-fA-F]{"+j.value.length+"}$"));if(m.test(j.value)){l=j.value;i(k,j.value);}else{j.value=l;i(k,'');}});}};},null);
__d("DeveloperAppOptionalField",["CSS","cx","DOM","Event"],function(a,b,c,d,e,f,g,h,i,j){var k={init:function(l,m,n){n=n||[];var o=i.find(l,'input');this.updateVisibility(o,m,n);m.concat(n).forEach(function(p){return g.addClass(p,"_5tcg");});j.listen(o,'click',function(){k.updateVisibility(o,m,n);});},_updateVisibilityElems:function(l,m){m.forEach(function(n){return g.conditionClass(n,"_5q5h",l);});},updateVisibility:function(l,m,n){this._updateVisibilityElems(!l.checked,m);this._updateVisibilityElems(l.checked,n);}};e.exports=k;},null);
__d("DeveloperCategorySelector",["CSS"],function(a,b,c,d,e,f,g){var h={init:function(i,j,k){this.handleChange(i,j,k);i.subscribe('change',this.handleChange.bind(this,i,j,k));},handleChange:function(i,j,k){if(i.getValue()!=k){g.hide(j);}else g.show(j);}};e.exports=h;},null);
__d("DeveloperAsyncDialog",["AsyncRequest","copyProperties","CSS","cx","DOM"],function(a,b,c,d,e,f,g,h,i,j,k){function l(m){this.dialog=m;this.statusElement=k.find(m.getRoot(),'.'+"_2uf");this.contentElement=k.find(m.getRoot(),'.'+"_2ug");}h(l.prototype,{show:function(m,n){this.showStatus();this.dialog.show();new g(m).setMethod('POST').setData(n).setHandler(this._handleData.bind(this)).send();},_handleData:function(m){k.setContent(this.contentElement,m.getPayload());this.hideStatus();},hideStatus:function(m,n){i.show(this.contentElement);i.hide(this.statusElement);},showStatus:function(m,n){i.hide(this.contentElement);i.show(this.statusElement);}});e.exports=l;},null);
__d("DeveloperAppInlineValidation",["CSS","DOM","Event","Parent","Vector","DOMScroll","cx"],function(a,b,c,d,e,f,g,h,i,j,k,l,m){var n=150,o={registerPopup:function(p,q){i.listen(p,'mouseenter',function(event){var r=j.byClass(p,"_5b_j");if(g.hasClass(r,'valid')||!r.hasOwnProperty('_errors'))return;var s=h.find(q.getContent(),'div.'+"_5p3t");h.setContent(s,h.create('ul',{},r._errors.map(function(t){return h.create('li',{},t);})));});},isInvalid:function(p){return g.hasClass(p,'validated')&&!g.hasClass(p,'valid');},isValid:function(p){return g.hasClass(p,'validated')&&g.hasClass(p,'valid');},setValid:function(p){g.addClass(p,'validated');g.addClass(p,'valid');},setInvalid:function(p,q){p._errors=q;g.addClass(p,'validated');g.removeClass(p,'valid');},clearFlags:function(p){g.removeClass(p,'validated');g.removeClass(p,'valid');},show:function(p,q){if(!q)q=document;var r=null;h.scry(q,'div.'+"_5b_j").forEach(function(s){var t=s.firstChild.firstChild,u=t.getAttribute('class');if(u&&u.indexOf("_59fh")!==-1)t=t.firstChild;var v=t.getAttribute('name');if(!v)v=t.getAttribute('id');if(p.hasOwnProperty(v)){this.setInvalid(s,p[v]);var w=k.getElementPosition(s);if(!r||w<r)r=w;return;}this.clearFlags(s);}.bind(this));if(r){r.y-=n;l.scrollTo(r);}},updateValidations:function(p,q){if(!q)q=document;var r=h.scry(q,'div.'+"_5b_j").some(function(s){return g.hasClass(s,'validated');});if(!r)return;this.show(p,q);},clearAllValidations:function(p){if(!p)p=document;h.scry(p,'div.'+"_5b_j").forEach(function(q){return this.clearFlags(q);}.bind(this));}};e.exports=o;},null);
__d("cssVar",[],function(a,b,c,d,e,f){function g(h){throw new Error('cssVar'+'("'+h+'"): Unexpected class transformation.');}e.exports=g;},null);
__d("DeveloperAppPlatformSelector",["React","ReactPropTypes","Animation","CSS","DeveloperAppPlatform","Grid.react","XUIText.react","cx","cssVar"],function(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o){var p=l.GridItem,q=g.createClass({displayName:'DeveloperAppPlatformSelector',propTypes:{onSelect:h.func.isRequired,animateOnSelect:h.bool,visiblePlatforms:h.array.isRequired,disabledPlatforms:h.array},_onClick:function(r){if(this._platformDisabled(r))return;if(this.props.animateOnSelect===false)return this.props.onSelect(r);var s=this.refs[(r+"-image")].getDOMNode();j.toggle(this.refs[(r+"-selected-image")].getDOMNode());j.toggleClass(s,'active');new i(s).to('opacity',0).duration(600).ondone(function(){return this.props.onSelect(r);}.bind(this)).go();new i(this.refs[(r+"-label")].getDOMNode()).to('color',"#5890ff").duration(600).go();},_platformDisabled:function(r){return (this.props.disabledPlatforms&&this.props.disabledPlatforms.indexOf(r)!=-1);},render:function(){var r=this.props.visiblePlatforms.map(function(s){return (p({className:(("_5s9l")+(this._platformDisabled(s)?' '+"disabled":'')),onClick:this._onClick.bind(this,s),key:s},g.DOM.div({className:"_5uuz"},g.DOM.div({'data-platform':s,className:"_5s9m selected active hidden_elem",ref:(s+"-selected-image")}),g.DOM.div({'data-platform':s,className:"_5s9m",ref:(s+"-image")})),m({weight:"bold",size:"medium",display:"block",className:"mtm",ref:(s+"-label")},k.labels[s])));}.bind(this));return (l({cols:4,fixed:true,className:"_5s9o"},r));}});e.exports=q;},null);
__d("DevsiteBasicSettings",["CSS","DOM","DOMScroll","Vector","$","cx"],function(a,b,c,d,e,f,g,h,i,j,k,l){var m={updateAddPlatformButton:function(){g.conditionShow(k('add-platform-button'),m.getHiddenPlatformCards().length);},getAddedPlatformCards:function(){return h.scry(document,'div.'+"_5t1j"+':not(.hidden_elem)');},getHiddenPlatformCards:function(){return h.scry(document,'div.'+"_5t1j"+'.hidden_elem');},removePlatform:function(n){g.hide(n);m.getPlatformCheckbox(n).checked=false;m.updateAddPlatformButton();},addPlatform:function(n){var o=h.find(document,'div.'+m.getPlatformCardClass(n));g.show(o);m.getPlatformCheckbox(o).checked=true;m.updateAddPlatformButton();var p=j.getScrollPosition(),q=j.getElementPosition(o),r=new j(p.x,q.y-100,'document');i.scrollTo(r);g.addClass(o,"_5tqe");g.addClass(o,"_5tqf");setTimeout(function(){g.removeClass(o,"_5tqf");},4000);},getPlatformCheckbox:function(n){return h.find(n,'input[type="checkbox"][name="platforms[]"]');},getPlatformCardClass:function(n){switch(n){case 'android':return "_59f_";case 'ios':return "_59g1";case 'playstation':return "_59g5";case 'xbox':return "_59g6";case 'web':return "_59g7";case 'canvas':return "_59g9";case 'windows':return "_59gt";case 'page_tab':return "_59g-";}}};e.exports=m;},null);
__d("DeveloperAppPlatformSelectorDialog",["React","ReactLayeredComponentMixin","DeveloperAppPlatformSelector","DevsiteBasicSettings","Image.react","ReactPropTypes","XUIButton.react","XUIDialog.react","XUIDialogBody.react","XUIDialogCancelButton.react","XUIDialogFooter.react","XUIDialogTitle.react","fbt","cx","ix"],function(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u){var v=g.createClass({displayName:'DeveloperAppPlatformSelectorDialog',mixins:[h],getInitialState:function(){return {dialogShown:false,disabledPlatforms:[]};},propTypes:{visiblePlatforms:l.array.isRequired},render:function(){return (m({type:"button",size:"xxlarge",rel:"async",image:k({src:u('/images/ui/x/image/plus.png')}),label:"Add Platform",onClick:function(event){return this.show();}.bind(this)}));},_onSelect:function(w){this.hide();setTimeout(function(){j.addPlatform(w);},450);},renderLayers:function(){return {dialog:n({shown:this.state.dialogShown,width:680,modal:true,layerHideOnBlur:false},r({showCloseButton:false},"Select Platform"),o({className:"_5t1i"},i({onSelect:this._onSelect,visiblePlatforms:this.props.visiblePlatforms,disabledPlatforms:this.state.disabledPlatforms,animateOnSelect:false})),q(null,p(null)))};},show:function(){var w=[];j.getAddedPlatformCards().forEach(function(x){return w.push(j.getPlatformCheckbox(x).getAttribute('value'));});this.setState({dialogShown:true,disabledPlatforms:w});},hide:function(){this.setState({dialogShown:false});}});e.exports=v;},null);
__d("DevsiteButtonTextField.react",["AsyncRequest","ReactPropTypes","React","URI","XUIButton.react","cx","fbt"],function(a,b,c,d,e,f,g,h,i,j,k,l,m){var n=i.createClass({displayName:'DevsiteButtonTextField',propTypes:{value:h.string,buttonHidden:h.bool,buttonLabel:h.string,action:h.string,actionType:h.oneOf(['async','link'])},getInitialState:function(){return {value:this.props.value,buttonHidden:this.props.buttonHidden,buttonLabel:this.props.buttonLabel,action:this.props.action,actionType:this.props.actionType};},getDefaultProps:function(){return {buttonHidden:false,buttonLabel:"Show"};},_defaultClickHandler:function(event){if(this.state.action)if(this.state.actionType=='link'){j(this.state.action).go(true);}else new g(this.state.action).setMethod('POST').setHandler(function(o){var p=true;if(typeof o.payload.buttonHidden!=='undefined')p=o.payload.buttonHidden;var q=this.state.action;if(typeof o.payload.action!=='undefined')q=o.payload.action;this.setState({buttonLabel:o.payload.buttonLabel,buttonHidden:p,value:o.payload.value,action:q});}.bind(this)).send();return event.preventDefault();},render:function(){var o=this.state.value,p=null;if(!this.state.buttonHidden)p=k({type:"button",label:this.state.buttonLabel,onClick:this.props.onClick||this._defaultClickHandler});return (i.DOM.div({className:"_5e_6 _5rvc"},i.DOM.span({className:"_5rv9"},o),p));}});e.exports=n;},null);
__d("DevsiteExternalPlatformCard",["DevsiteBasicSettings","Event"],function(a,b,c,d,e,f,g,h){function i(j,k){"use strict";h.listen(k,'click',function(event){g.removePlatform(j);return false;});}e.exports=i;},null);
__d("DevsiteNullSearchSource",["AbstractSearchSource","SearchableEntry"],function(a,b,c,d,e,f,g,h){for(var i in g)if(g.hasOwnProperty(i))k[i]=g[i];var j=g===null?null:g.prototype;k.prototype=Object.create(j);k.prototype.constructor=k;k.__superConstructor__=g;function k(){"use strict";if(g!==null)g.apply(this,arguments);}k.prototype.searchImpl=function(l,m,n){"use strict";l=l.trim();if(l==='')return;var o=new h({uniqueID:l,title:l});m([o],l);};e.exports=k;},null);
__d("RegexMatchSearchSource",["AbstractSearchSource","SearchableEntry"],function(a,b,c,d,e,f,g,h){for(var i in g)if(g.hasOwnProperty(i))k[i]=g[i];var j=g===null?null:g.prototype;k.prototype=Object.create(j);k.prototype.constructor=k;k.__superConstructor__=g;function k(l){"use strict";this.$RegexMatchSearchSource0=new RegExp(l);}k.prototype.searchImpl=function(l,m,n){"use strict";if(this.$RegexMatchSearchSource0.test(l)){var o=new h({uniqueID:l,title:l});m([o],l);return;}m([],l);};e.exports=k;},null);
__d("BonsaiDevsiteStyle",[],function(a,b,c,d,e,f){var g={CHART_WIDTH:700,BREAKDOWN_WIDTH:550,TIME_SELECTOR_WIDTH:560};e.exports=g;},null);